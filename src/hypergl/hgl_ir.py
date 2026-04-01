import re
import struct
import hypergl
from hypergl import CommandBuffer, SceneCompiler
from typing import NamedTuple, Any

ASSIGN_MAP = {
    '+=': 'add',  '-=': 'sub', 
    '*=': 'mul',  '/=': 'div',
    '&=': 'and',  '|=': 'or',  '^=': 'xor',
    '<<=': 'lsh', '>>=': 'rsh'
}

class Token(NamedTuple):
    type: str | None
    value: str
    line: int
    column: int
    start_pos: int # Byte offset in source

class HGLCompiler:
    def __init__(self, cmd: CommandBuffer, env: dict):
        self.cmd = cmd
        self.sc = SceneCompiler(cmd)
        self.env = env
        self.source = ""
        self.source_lines = []
        self.tokens = []
        self.pos = 0
        self.aliases = {} 
        self.defines = {} 

        self._init_registers()

    def _init_registers(self):
        self.user_vars = {} 
        # i0-i5 are for users. i6-i7 are reserved for compiler logic.
        self.available_regs = [0, 1, 2, 3, 4, 5] 
        self.next_reg_idx = 0

    def _alloc_reg(self, name):
        if name in self.user_vars:
            return self.user_vars[name]
        if self.next_reg_idx >= len(self.available_regs):
            raise IndexError("HGL-IR: Out of registers! (Maximum 6 user variables)")
        reg = self.available_regs[self.next_reg_idx]
        self.user_vars[name] = reg
        self.next_reg_idx += 1
        return reg

    def _get_reg(self, name_or_reg):
        """Resolves 'my_var' or 'i0' to an integer index."""
        if name_or_reg in self.user_vars:
            return self.user_vars[name_or_reg]
        if name_or_reg in self.aliases:
            return self.aliases[name_or_reg]
        if name_or_reg.startswith('i'):
            return int(name_or_reg[1])
        self._fail(f"Undefined variable or register: {name_or_reg}")

    def _parse_reg_or_var(self):
        """
        Parses a register (i0), an alias, OR a user-defined variable (time_now).
        Returns the integer register index.
        """
        tk = self._peek()
        if not tk: self._fail("Unexpected EOF")

        # 1. Alias Check
        if tk.type == 'ID' and tk.value in self.aliases:
            self._consume()
            return self.aliases[tk.value]
        
        # 2. User Variable Check
        if tk.type == 'ID' and tk.value in self.user_vars:
            self._consume()
            return self.user_vars[tk.value]

        # 3. Raw Register Check (i0..i7)
        if tk.type == 'REG':
            self._consume()
            return int(tk.value[1:])
            
        self._fail(f"Expected register or variable, got {tk.type} ('{tk.value}')", tk)


    def compile(self, source_code: str):
        # 1. Preprocess
        expanded = self._preprocess(source_code)
        final_source = self._apply_defines(expanded)
        
        # 2. Tokenize
        raw_tokens = self._tokenize(final_source)
        
        # 3. Optimize (Dead Code Elimination)
        self.tokens = self._dce_tokens(raw_tokens)
        
        # 4. Parse / Emit
        self.pos = 0
        while self.pos < len(self.tokens):
            self._parse_statement()

    def _dce_tokens(self, tokens):
        """
        Performs basic Dead Code Elimination.
        Removes instructions that are unreachable.
        """
        optimized = []
        i = 0
        depth = 0 # Track block depth for control flow
        is_dead = False # Flag for unreachable code

        while i < len(tokens):
            tk = tokens[i]
            
            # If we are in a dead block, we only look for the closing brace or a label
            # Note: HGL-IR doesn't have explicit GOTO in source, so labels are implicit in blocks.
            # But 'ret' makes subsequent code in the same block dead.
            
            if is_dead:
                # If we see a closing brace, we might be exiting the dead block scope
                if tk.type == 'RBRACE':
                    depth -= 1
                    if depth < 0: # Should not happen in valid code, but safety check
                        is_dead = False
                    # Keep the brace to maintain structure integrity for the parser
                    optimized.append(tk)
                elif tk.type == 'LBRACE':
                    depth += 1 # Nested dead code
                    
                # Skip everything else
                i += 1
                continue

            # --- Check for Terminators ---
            if tk.type == 'ID' and tk.value == 'ret':
                optimized.append(tk)
                # Everything after 'ret' in the current block is dead
                is_dead = True
                depth = 0 # Reset depth tracking for the dead region
                i += 1
                continue
                
            # --- Normal Flow ---
            if tk.type == 'LBRACE':
                depth += 1
            elif tk.type == 'RBRACE':
                depth -= 1
                
            optimized.append(tk)
            i += 1
            
        return optimized
            
    # --- Error Reporting ---
    def _fail(self, msg: str, token: Token | None = None):
        """Generates a GCC/Python-style error message with context."""
        if token:
            line_idx = token.line - 1
            col = token.column
        else:
            # EOF error
            line_idx = len(self.source_lines) - 1
            col = len(self.source_lines[line_idx]) if self.source_lines else 0

        # Grab context
        line_text = self.source_lines[line_idx] if 0 <= line_idx < len(self.source_lines) else ""
        
        # Format:
        # SyntaxError: Unknown opcode: 'flarg'
        #   Line 4:   flarg i0, 10
        #             ^
        
        pointer = " " * col + "^"
        err_str = (
            f"{msg}\n"
            f"  Line {token.line if token else 'EOF'}:\n"
            f"    {line_text}\n"
            f"    {pointer}"
        )
        raise SyntaxError(err_str)

    # --- Pass 1: Directive Resolution ---
    def _preprocess(self, source, depth=0):
        if depth > 10: raise RecursionError("HGL-IR: #include depth exceeded (Circular reference?)")
        
        lines = source.splitlines()
        processed_lines = []
        
        for line in lines:
            # Handle #define (Name must be uppercase for clarity)
            # Syntax: #define KEY VALUE
            define_match = re.match(r'^\s*#define\s+([A-Z0-9_]+)\s+(.+)$', line)
            if define_match:
                key, val = define_match.groups()
                self.defines[key] = val.strip()
                continue
                
            # Handle #include
            # Syntax: #include "filename"
            include_match = re.match(r'^\s*#include\s+["<](.+)[">]$', line)
            if include_match:
                fname = include_match.group(1)
                content = self.env.get(fname)
                if content is None:
                    raise FileNotFoundError(f"HGL-IR: Included resource @{fname} not found in environment.")
                processed_lines.append(self._preprocess(content, depth + 1))
                continue
                
            processed_lines.append(line)
            
        return "\n".join(processed_lines)

    # --- Pass 2: Safe Substitution ---
    def _apply_defines(self, source):
        if not self.defines:
            return source
            
        # Sort keys by length descending to prevent partial replacement 
        # (though \b usually handles this, it's a "Pro" defensive move)
        keys = sorted(self.defines.keys(), key=len, reverse=True)
        
        for key in keys:
            pattern = r'\b' + re.escape(key) + r'\b'
            source = re.sub(pattern, str(self.defines[key]), source)
        return source

    # --- Tokenizer ---
    def _tokenize(self, code):
        token_spec = [
            ('COMMENT',     r'(#|//).*'),
            ('HEX',         r'0x[0-9a-fA-F]+'),
            ('FLOAT',       r'-?(?:\d+\.\d*|\.\d+)(?:[eE][-+]?\d+)?|\d+[eE][-+]?\d+'),
            ('INT',         r'-?\d+'),
            ('REG',         r'i\d+'),
            ('REF',         r'@[a-zA-Z0-9_]+'),
            ('DOT',         r'\.'),
            ('STRING',      r'"[^"\n]*"'),
            ('MATH_ASSIGN', r'(<<=|>>=|\+=|-=|\*=|/=|&=|\|=|\^=|=)'),
            ('ID',          r'[a-zA-Z_][a-zA-Z0-9_]*'),
            ('OP',          r'(==|!=|<=|>=|<|>)'),
            ('LBRACE',      r'\{'),
            ('RBRACE',      r'\}'),
            ('LPAREN',      r'\('), # NEW
            ('RPAREN',      r'\)'), # NEW
            ('COMMA',       r','),
            ('NEWLINE',     r'\n'),
            ('SKIP',        r'[ \t]+'),
            ('MISMATCH',    r'.'),
        ]
        
        tok_regex = '|'.join('(?P<%s>%s)' % pair for pair in token_spec)
        
        tokens = []
        line_num = 1
        line_start = 0

        for m in re.finditer(tok_regex, code):
            kind = m.lastgroup
            value = m.group()
            
            if kind == 'NEWLINE':
                line_num += 1
                line_start = m.end()
                continue
            elif kind == 'SKIP' or kind == 'COMMENT':
                continue
            elif kind == 'MISMATCH':
                # Generate a temporary token just for the error message
                bad_tk = Token(kind, value, line_num, m.start() - line_start, m.start())
                self._fail(f"Unexpected character '{value}'", bad_tk)
            
            column = m.start() - line_start
            tokens.append(Token(kind, value, line_num, column, m.start()))
            
        return tokens
    
    def _resolve_uniform(self, pipeline, name):
        """
        Reflects into the HyperGL Pipeline to find uniform Location and Type.
        Uses the public hypergl.inspect() API.
        """
        import hypergl
        
        # 1. Get reflection data dictionary
        info = hypergl.inspect(pipeline)
        
        # 2. Access the 'interface' key 
        # Structure: {'type': 'pipeline', 'interface': (attribs, uniforms, blocks), ...}
        if 'interface' not in info or not info['interface']:
             # Fallback for compute or missing link
             if 'program' in info:
                 # Try inspecting the program object directly if exposed?
                 # Actually, inspect(pipeline) already returns the program interface.
                 pass
             self._fail(f"Pipeline has no interface data. Link failed?")

        # 3. Get Uniforms List (Index 1 of the tuple)
        # interface tuple: (attributes_list, uniforms_list, blocks_list)
        interface = info['interface']
        if not interface or len(interface) < 2:
            self._fail(f"Invalid interface format for pipeline")
            
        uniforms = interface[1] # List of dicts
        
        for u in uniforms:
            if u['name'] == name:
                loc = u['location']
                gl_type = u['gltype']
                
                # Map GL Enum to VM Type ID
                # 0=float, 1=int, 2=uint
                vm_type = 0
                if gl_type in (0x1404, 0x8B53, 0x8B54): # GL_INT...
                    vm_type = 1
                elif gl_type in (0x1405, 0x8DC6, 0x8DC7): # GL_UNSIGNED_INT...
                    vm_type = 2
                
                return loc, vm_type

        self._fail(f"Uniform '{name}' not found in pipeline")

    # --- Helpers ---
    def _peek(self):
        return self.tokens[self.pos] if self.pos < len(self.tokens) else None

    def _consume(self, type_name=None):
        token = self._peek()
        if not token: 
            self._fail("Unexpected End of File")
        
        if type_name and token.type != type_name:
            self._fail(f"Expected {type_name}, got {token.type} ('{token.value}')", token)
            
        self.pos += 1
        return token

    def _parse_val(self):
        t = self._consume()
        if t.type == 'INT': return int(t.value)
        if t.type == 'HEX': return int(t.value, 16)
        if t.type == 'FLOAT': return struct.unpack('I', struct.pack('f', float(t.value)))[0]
        self._fail(f"Expected number, got {t.type}", t)

    def _resolve(self):
        name = self._consume('REF').value[1:]
        if name not in self.env: 
            # We can point to the token that caused the reference error
            self._fail(f"Undefined resource: @{name}", self.tokens[self.pos-1])
        return self.env[name]
    
    def _parse_var_declaration(self):
        self._consume('ID') # Consume 'var'
        
        targets = []
        if self._peek().type == 'LPAREN':
            # Case: var (s, c) = ...
            self._consume('LPAREN')
            while self._peek().type != 'RPAREN':
                targets.append(self._consume('ID').value)
                if self._peek().type == 'COMMA': self._consume('COMMA')
            self._consume('RPAREN')
        else:
            # Case: var x = ...
            targets.append(self._consume('ID').value)

        self._consume('MATH_ASSIGN') # Consume '='
        
        # --- RHS Logic ---
        rhs_name = self._consume('ID').value
        
        if self._peek() and self._peek().type == 'LPAREN':
            # Function Call Path: var x = time()
            self._consume('LPAREN'); self._consume('RPAREN')
            
            if rhs_name == 'time':
                reg = self._alloc_reg(targets[0])
                self.cmd.get_time(reg)
            elif rhs_name == 'delta':
                reg = self._alloc_reg(targets[0])
                self.cmd.get_delta(reg)
            elif rhs_name == 'rand':
                reg = self._alloc_reg(targets[0])
                self.cmd.gen_rand(reg)
            elif rhs_name == 'sincos':
                # var (s, c) = sincos(input_var)
                # Wait, we need the argument. Let's adjust.
                pass # See updated logic below
        else:
            # Simple Assignment: var x = i0 or var x = 10.0
            pass
    
    def _parse_assignment(self):
        # 1. Parse LHS - Use the new flexible parser!
        dest_reg = self._parse_reg_or_var()  # CHANGED FROM _parse_reg()
        
        # 2. Parse Operator
        op_tok = self._consume('MATH_ASSIGN')
        operator = op_tok.value
        
        # 3. Parse RHS
        rhs_tok = self._peek()
        is_reg_source = False
        src_val = 0
        src_reg = 0

        # Check if RHS is a register, alias, OR variable
        if rhs_tok.type == 'REG' or (rhs_tok.type == 'ID' and (rhs_tok.value in self.aliases or rhs_tok.value in self.user_vars)):
            src_reg = self._parse_reg_or_var() # CHANGED FROM _parse_reg()
            is_reg_source = True
        else:
            src_val = self._parse_val()
            is_reg_source = False


        # --- Generate Code ---

        # Handle basic assignment (=) separately from math
        if operator == '=':
            if is_reg_source:
                # reg = reg (Use add 0 or copy)
                self.cmd.alu(dest_reg, src_reg, 'add') # Assuming add(a, b) -> a += b, so this effectively copies if dest was 0? 
                # Correction: HGL-IR ALU is "DEST = DEST op SRC".
                # To do "i0 = i1", we can't just use add. 
                # We usually do: "mov i0, 0" then "add i0, i1".
                # OR if the VM supports `mov reg, reg`.
                # Let's assume your existing 'mov' logic handles this:
                self.cmd.set_iter(dest_reg, 0) # Clear dest
                self.cmd.alu(dest_reg, src_reg, 'add') # Add src to 0
            else:
                # reg = immediate
                self.cmd.set_iter(dest_reg, src_val)
            return

        # Handle Math (+=, -=, etc.)
        vm_op = ASSIGN_MAP.get(operator)
        if not vm_op:
            self._fail(f"Unknown operator {operator}", op_tok)

        if is_reg_source:
            # i0 += i1
            self.cmd.alu(dest_reg, src_reg, vm_op)
        else:
            SCRATCH_REG = 7 
            if dest_reg == SCRATCH_REG:
                 self._fail("Cannot use i7 with immediate math (reserved for scratch)", op_tok)

            self.cmd.set_iter(SCRATCH_REG, src_val)
            self.cmd.alu(dest_reg, SCRATCH_REG, vm_op)
            
    def _handle_var(self):
        self._consume('ID') # 'var'
        
        # 1. Parse LHS: var x = ... OR var (a, b) = ...
        names = []
        is_tuple = False
        if self._peek().type == 'LPAREN':
            is_tuple = True
            self._consume('LPAREN')
            while self._peek().type != 'RPAREN':
                names.append(self._consume('ID').value)
                if self._peek().type == 'COMMA': self._consume('COMMA')
            self._consume('RPAREN')
        else:
            names.append(self._consume('ID').value)

        self._consume('MATH_ASSIGN') # '='
        
        # 2. Parse RHS
        rhs_tk = self._consume('ID')
        rhs_name = rhs_tk.value
        
        if self._peek() and self._peek().type == 'LPAREN':
            # --- Function Call Path: var x = time() ---
            self._consume('LPAREN')
            args = []
            while self._peek().type != 'RPAREN':
                # Args can be numbers or existing variables
                if self._peek().type in ('INT', 'FLOAT', 'HEX'):
                    args.append({'type': 'val', 'val': self._parse_val()})
                else:
                    args.append({'type': 'reg', 'val': self._parse_reg_or_var()})
                
                if self._peek().type == 'COMMA': self._consume('COMMA')
            self._consume('RPAREN')

            # Dispatch
            if rhs_name == 'time':
                r = self._alloc_reg(names[0])
                self.cmd.get_time(r)
            elif rhs_name == 'delta':
                r = self._alloc_reg(names[0])
                self.cmd.get_delta(r)
            elif rhs_name == 'rand':
                r = self._alloc_reg(names[0])
                self.cmd.gen_rand(r)
            elif rhs_name == 'sincos':
                # var (s, c) = sincos(in)
                if len(names) != 2: self._fail("sincos returns (sin, cos)", rhs_tk)
                
                # Resolve input argument
                arg = args[0]
                r_in = arg['val']
                if arg['type'] == 'val':
                    # Load immediate into scratch i6
                    self.cmd.set_iter(6, r_in)
                    r_in = 6
                
                rs = self._alloc_reg(names[0])
                rc = self._alloc_reg(names[1])
                self.cmd.sin_cos(r_in, rs, rc)
            else:
                self._fail(f"Unknown function '{rhs_name}'", rhs_tk)
        else:
            # --- Simple Assignment: var x = i0 OR var x = 100 ---
            # NOTE: We consumed the ID for RHS name logic, but if it was a register or alias
            # we need to handle that. This simple parser assumes func calls for now.
            # For "var x = y", you'd need to peek/backtrack or handle ID logic here.
            # A simple implementation for 'mov':
            
            dest = self._alloc_reg(names[0])
            # Check if rhs_name is actually a register or number?
            # Since we consumed it as ID, we check aliases/vars:
            if rhs_name in self.user_vars:
                src = self.user_vars[rhs_name]
                self.cmd.set_iter(dest, 0); self.cmd.alu(dest, src, 'add')
            elif rhs_name in self.aliases:
                src = self.aliases[rhs_name]
                self.cmd.set_iter(dest, 0); self.cmd.alu(dest, src, 'add')
            else:
                self._fail(f"Invalid assignment source '{rhs_name}'", rhs_tk)

    def _parse_property_set(self):
        # 1. Parse Object (@my_pipe)
        obj = self._resolve() # Consumes REF
        
        # 2. Parse Dot and Property (.u_Offset)
        self._consume('DOT')
        prop_name = self._consume('ID').value
        
        # 3. Parse Assignment (=)
        op = self._consume('MATH_ASSIGN')
        if op.value != '=':
            self._fail("Uniforms only support direct assignment '='", op)
            
        # 4. Parse Value (Register or Number)
        # Use simple register parsing for now
        reg = self._parse_reg_or_var() 

        # 5. Compile-Time Reflection
        if isinstance(obj, hypergl.Pipeline) or isinstance(obj, hypergl.Compute):
            loc, vm_type = self._resolve_uniform(obj, prop_name)
            
            # OPTIMIZATION: Pass the raw integer vm_type directly to C
            self.cmd.set_uniform(loc, reg, vm_type)
        else:
            self._fail(f"Cannot set property on type {type(obj)}")

    # --- Parser ---
    def _parse_statement(self):
        # 1. Handle Assignments (x += 1) or Vars
        head = self._peek()
        if not head: return

        if head.type == 'ID' and head.value == 'var':
            self._handle_var()
            return

        # Check for assignment start (REG or VAR)
        if head.type == 'REG' or (head.type == 'ID' and (head.value in self.aliases or head.value in self.user_vars)):
            self._parse_assignment()
            return
        
        if head.type == 'REF':
            self._parse_property_set()
            return

        # 2. Standard Opcodes
        tk = self._consume('ID')
        op = tk.value

        # --- Data Movement ---
        if op == 'mov':
            reg = self._parse_reg_or_var(); self._consume('COMMA')
            nxt = self._peek()
            if nxt and (nxt.type in ('REG', 'ID')):
                src = self._parse_reg_or_var()
                self.cmd.set_iter(reg, 0)
                self.cmd.alu(reg, src, 'add')
            else:
                val = self._parse_val()
                self.cmd.set_iter(reg, val)
        
        elif op == 'alias':
            name = self._consume('ID').value
            op_tok = self._consume('MATH_ASSIGN')
            if op_tok.value != '=': self._fail("Expected '='", op_tok)
            self.aliases[name] = self._parse_reg_or_var()

        # --- Math (ALU) ---
        elif op in ('add', 'sub', 'mul', 'div', 'and', 'or', 'xor', 'lsh', 'rsh', 'fadd', 'fsub', 'fmul', 'fdiv'):
            dest = self._parse_reg_or_var(); self._consume('COMMA')
            src = self._parse_reg_or_var()
            self.cmd.alu(dest, src, op)
            
        elif op == 'sincos':
            rin = self._parse_reg_or_var(); self._consume('COMMA')
            rsin = self._parse_reg_or_var(); self._consume('COMMA')
            rcos = self._parse_reg_or_var()
            self.cmd.sin_cos(rin, rsin, rcos)

        # --- Comparison (Explicit Types) ---
        elif op == 'cmp': # Integer
            dest = self._parse_reg_or_var(); self._consume('COMMA')
            ra = self._parse_reg_or_var();   self._consume('COMMA')
            
            nxt = self._peek()
            if nxt and (nxt.type in ('REG', 'ID')):
                rb = self._parse_reg_or_var()
            else:
                val = self._parse_val(); self.cmd.set_iter(7, val); rb = 7
            
            self._consume('COMMA')
            cond = self._consume('STRING').value.strip('"') if self._peek().type == 'STRING' else self._consume('OP').value
            self.cmd.cmp(dest, ra, rb, cond)

        elif op == 'fcmp': # Float
            dest = self._parse_reg_or_var(); self._consume('COMMA')
            ra = self._parse_reg_or_var();   self._consume('COMMA')
            
            nxt = self._peek()
            if nxt and (nxt.type in ('REG', 'ID')):
                rb = self._parse_reg_or_var()
            else:
                val = self._parse_val(); self.cmd.set_iter(7, val); rb = 7
            
            self._consume('COMMA')
            cond = self._consume('STRING').value.strip('"') if self._peek().type == 'STRING' else self._consume('OP').value
            self.cmd.fcmp(dest, ra, rb, cond)

        # --- Control Flow ---
        elif op == 'if': # Integer IF
            ra = self._parse_reg_or_var()
            tk = self._peek()
            cond = self._consume('STRING').value.strip('"') if tk and tk.type == 'STRING' else self._consume('OP').value
            
            nxt = self._peek()
            if nxt and (nxt.type in ('REG', 'ID')):
                rb = self._parse_reg_or_var()
            else:
                val = self._parse_val(); self.cmd.set_iter(6, val); rb = 6

            self._consume('LBRACE')
            self.cmd.cmp(7, ra, rb, cond)
            with self.sc.reg_condition(7):
                while self._peek() and self._peek().type != 'RBRACE': self._parse_statement()
            self._consume('RBRACE')

        elif op == 'fif': # Float IF
            ra = self._parse_reg_or_var()
            tk = self._peek()
            cond = self._consume('STRING').value.strip('"') if tk and tk.type == 'STRING' else self._consume('OP').value
            
            nxt = self._peek()
            if nxt and (nxt.type in ('REG', 'ID')):
                rb = self._parse_reg_or_var()
            else:
                val = self._parse_val(); self.cmd.set_iter(6, val); rb = 6

            self._consume('LBRACE')
            self.cmd.fcmp(7, ra, rb, cond) # Uses FCMP
            with self.sc.reg_condition(7):
                while self._peek() and self._peek().type != 'RBRACE': self._parse_statement()
            self._consume('RBRACE')

        # --- Graphics ---
        elif op == 'clear': self.cmd.clear()
        elif op == 'bind':
            obj = self._resolve()
            if isinstance(obj, hypergl.Pipeline): self.cmd.bind_pipeline(obj)
            elif isinstance(obj, hypergl.DescriptorSet): self.cmd.bind_descriptor_set(obj)
            else: self._fail(f"Cannot bind {type(obj)}")
            
        elif op == 'uniform':
            pipe = self._resolve(); self._consume('COMMA')
            name = self._consume('STRING').value.strip('"'); self._consume('COMMA')
            reg = self._parse_reg_or_var(); self._consume('COMMA')
            utype = self._consume('STRING').value.strip('"')
            self.sc.inject_uniform(pipe, name, reg, utype)

        elif op == 'draw':
            count, inst = -1, -1
            if self._peek() and self._peek().type == 'INT':
                count = int(self._consume().value)
                if self._peek() and self._peek().type == 'COMMA':
                    self._consume(); inst = int(self._consume().value)
            self.cmd.draw(count, inst)

        elif op == 'time':
            # Syntax: time REG
            self.cmd.get_time(self._parse_reg_or_var())
            
        elif op == 'delta':
            # Syntax: delta REG
            self.cmd.get_delta(self._parse_reg_or_var())
            
        elif op == 'rand':
            # Syntax: rand REG
            self.cmd.gen_rand(self._parse_reg_or_var())

        else:
            self._fail(f"Unknown opcode '{op}'", tk)