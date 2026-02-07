import re
import struct
import hypergl
from hypergl import CommandBuffer, SceneCompiler
from typing import NamedTuple, Any

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

    def compile(self, source_code: str):
        # Preprocessing
        expanded = self._preprocess(source_code)
        final_source = self._apply_defines(expanded)
        
        # Setup for error reporting
        self.source = final_source
        self.source_lines = final_source.split('\n')
        
        self.tokens = self._tokenize(final_source)
        self.pos = 0
        while self.pos < len(self.tokens):
            self._parse_statement()
            
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
            ('COMMENT', r'(#|//).*'),
            ('HEX',     r'0x[0-9a-fA-F]+'),
            ('FLOAT',   r'-?\d+\.\d+'),
            ('INT',     r'-?\d+'),
            ('REG',     r'i[0-7]'),
            ('REF',     r'@[a-zA-Z0-9_]+'),
            ('STRING',  r'"[^"]*"'),
            ('ID',      r'[a-zA-Z_][a-zA-Z0-9_]*'),
            ('OP',      r'(==|!=|<=|>=|<|>)'),
            ('ASSIGN',  r'='),
            ('LBRACE',  r'\{'),
            ('RBRACE',  r'\}'),
            ('COMMA',   r','),
            ('NEWLINE', r'\n'),
            ('SKIP',    r'[ \t]+'),
            ('MISMATCH',r'.'),
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

    # --- Parser Logic (Updated to use .type instead of .lastgroup) ---
    def _parse_reg(self):
        tk = self._peek()
        if tk and tk.type == 'ID' and tk.value in self.aliases:
            self._consume()
            return self.aliases[tk.value]
        
        t = self._consume('REG')
        return int(t.value[1]) # 'i0' -> 0

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

    # --- Parser ---
    def _parse_statement(self):
        tk = self._consume('ID')
        op = tk.value

        # --- Data Movement & Alias ---
        if op == 'mov':
            reg = self._parse_reg(); self._consume('COMMA')
            nxt = self._peek()
            # Handle "mov i0, i1" (Reg-to-Reg move via ADD 0)
            if nxt and (nxt.type == 'REG' or (nxt.type == 'ID' and nxt.value in self.aliases)):
                src = self._parse_reg()
                self.cmd.set_iter(reg, 0)
                self.cmd.alu(reg, src, 'add')
            else:
                # Handle "mov i0, 10" or "mov i0, 1.5"
                val = self._parse_val()
                self.cmd.set_iter(reg, val)
        
        elif op == 'alias':
            name = self._consume('ID').value; self._consume('ASSIGN')
            self.aliases[name] = self._parse_reg()

        # --- Math (ALU) ---
        elif op in ('add', 'sub', 'mul', 'div', 'and', 'or', 'xor', 'lsh', 'rsh', 'fadd', 'fsub', 'fmul', 'fdiv'):
            dest = self._parse_reg(); self._consume('COMMA')
            src = self._parse_reg()
            self.cmd.alu(dest, src, op)
            
        elif op == 'not':
            reg = self._parse_reg()
            self.cmd.alu(reg, reg, 'not')

        elif op == 'sincos':
            rin = self._parse_reg(); self._consume('COMMA')
            rsin = self._parse_reg(); self._consume('COMMA')
            rcos = self._parse_reg()
            self.cmd.sin_cos(rin, rsin, rcos)

        elif op == 'rand':
            self.cmd.gen_rand(self._parse_reg())

        # --- Graphics & Compute ---
        elif op == 'bind':
            obj = self._resolve()
            if isinstance(obj, hypergl.Pipeline): self.cmd.bind_pipeline(obj)
            elif isinstance(obj, hypergl.Compute): self.cmd.bind_compute(obj)
            elif isinstance(obj, hypergl.DescriptorSet): self.cmd.bind_descriptor_set(obj)
            else: raise TypeError(f"Cannot bind object of type {type(obj)}")

        elif op == 'draw':
            count, inst = -1, -1
            if self._peek() and self._peek().type == 'INT':
                count = int(self._consume().value)
                if self._peek() and self._peek().type == 'COMMA':
                    self._consume(); inst = int(self._consume().value)
            self.cmd.draw(count, inst)
            
        elif op == 'dispatch':
            x = int(self._consume('INT').value); self._consume('COMMA')
            y = int(self._consume('INT').value); self._consume('COMMA')
            z = int(self._consume('INT').value)
            self.cmd.dispatch(x, y, z)
            
        elif op == 'barrier': self.cmd.barrier()
        elif op == 'clear': self.cmd.clear()

        # --- Control Flow ---
        elif op == 'loop':
            count = int(self._consume('INT').value); self._consume('ID') # using
            reg = self._parse_reg(); self._consume('LBRACE')
            with self.sc.loop(reg, count):
                while self._peek() and self._peek().type != 'RBRACE': self._parse_statement()
            self._consume('RBRACE')

        elif op == 'if':
            ra = self._parse_reg()
            # Flexible operator parsing: allows both raw < and quoted "<"
            tk = self._peek()
            cond = self._consume('STRING').value.strip('"') if tk and tk.type == 'STRING' else self._consume('OP').value
            rb = self._parse_reg()
            self._consume('LBRACE')
            
            # Use i7 as scratchpad for result
            self.cmd.cmp(7, ra, rb, cond)
            with self.sc.reg_condition(7):
                while self._peek() and self._peek().type != 'RBRACE': self._parse_statement()
            self._consume('RBRACE')
            
        elif op == 'call':
            other = self._resolve()
            if not isinstance(other, hypergl.CommandBuffer): 
                self._fail(f"call target must be CommandBuffer, got {type(other)}", tk)
            self.cmd.call(other)
            
        elif op == 'ret': self.cmd.ret()

        # --- Memory ---
        elif op == 'store':
            reg = self._parse_reg(); self._consume('COMMA'); buf = self._resolve()
            self._consume('COMMA'); off = self._parse_val()
            self.cmd.store_reg(reg, buf, off)
            
        elif op == 'load':
            reg = self._parse_reg(); self._consume('COMMA'); buf = self._resolve()
            self._consume('COMMA'); off = self._parse_val()
            self.cmd.load_reg(reg, buf, off)
            
        elif op == 'store_indirect':
            val = self._parse_reg(); self._consume('COMMA'); buf = self._resolve()
            self._consume('COMMA'); idx = self._parse_reg()
            self._consume('COMMA'); stride = self._parse_val()
            self.cmd.store_reg_indirect(val, buf, idx, base_offset=0, stride=stride)

        elif op == 'load_indirect':
            dest = self._parse_reg(); self._consume('COMMA'); buf = self._resolve()
            self._consume('COMMA'); idx = self._parse_reg()
            self._consume('COMMA'); stride = self._parse_val()
            self.cmd.load_reg_indirect(dest, buf, idx, base_offset=0, stride=stride)

        elif op == 'copy':
            src = self._resolve(); self._consume('COMMA'); dst = self._resolve(); self._consume('COMMA')
            rs = self._parse_reg(); self._consume('COMMA'); rd = self._parse_reg(); self._consume('COMMA'); rz = self._parse_reg()
            self.cmd.copy_buffer(src, dst, rs, rd, rz)
            
        elif op == 'slide':
            buf = self._resolve(); self._consume('COMMA'); slot = self._parse_val(); self._consume('COMMA')
            reg = self._parse_reg(); self._consume('COMMA'); size = self._parse_val(); self._consume('COMMA')
            btype = self._consume('STRING').value.strip('"')
            self.cmd.set_buffer_offset(buf, slot, reg, size, btype)

        # --- Advanced AZDO ---
        elif op == 'draw_mdi_count':
            dbuf = self._resolve(); self._consume('COMMA'); cbuf = self._resolve()
            self._consume('COMMA'); max_c = self._parse_val()
            self.cmd.draw_indirect_count(buffer=dbuf, count_buffer=cbuf, max_count=max_c)

        elif op == 'uniform':
            pipe = self._resolve(); self._consume('COMMA')
            name = self._consume('STRING').value.strip('"'); self._consume('COMMA')
            reg = self._parse_reg(); self._consume('COMMA'); utype = self._consume('STRING').value.strip('"')
            self.sc.inject_uniform(pipe, name, reg, utype)

        # --- System ---
        elif op == 'time': self.cmd.get_time(self._parse_reg())
        elif op == 'delta': self.cmd.get_delta(self._parse_reg())
        
        elif op == 'assert':
            reg = self._parse_reg(); self._consume('COMMA'); val = self._parse_val(); self._consume('COMMA')
            tk = self._peek()
            cond = self._consume('STRING').value.strip('"') if tk and tk.type == 'STRING' else self._consume('OP').value
            self.cmd.assert_reg(reg, val, cond)
            
        elif op == 'print':
            msg = self._consume('STRING').value.strip('"')
            nxt = self._peek()
            if nxt and (nxt.type == 'REG' or (nxt.type == 'ID' and nxt.value in self.aliases)):
                self.cmd.print(msg, None, self._parse_reg())
            else: self.cmd.print(msg)

        else:
            self._fail(f"Unknown opcode '{op}'", tk)