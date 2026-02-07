import sys
import argparse
from hypergl.hgl_ir import HGLCompiler

# Mock CommandBuffer that does nothing
class MockCmd:
    def __getattr__(self, name):
        return lambda *args, **kwargs: None
    def __init__(self):
        self.ctx = None # Mock context

# Mock Compiler that only parses
class Linter(HGLCompiler):
    def __init__(self):
        # Pass a mock command buffer and an empty env
        # The env lookup will fail at runtime, so we override _resolve
        super().__init__(MockCmd(), {})
        self.undefined_refs = set()

    def _resolve(self):
        # Override to track references instead of crashing
        name = self._consume('REF').group()[1:]
        self.undefined_refs.add(name)
        return object() # Return dummy object

    def lint(self, code: str):
        try:
            self.compile(code)
            print("✅ Syntax OK")
            if self.undefined_refs:
                print("\nExternal References found (Require Python env):")
                for ref in sorted(self.undefined_refs):
                    print(f"  - @{ref}")
            return 0
        except Exception as e:
            print(f"❌ Error: {e}")
            return 1

def main():
    parser = argparse.ArgumentParser(description="HyperGL IR Static Linter")
    parser.add_argument("file", help="Path to .hgl file")
    args = parser.parse_args()

    try:
        with open(args.file, "r") as f:
            code = f.read()
    except FileNotFoundError:
        print(f"File not found: {args.file}")
        sys.exit(1)

    linter = Linter()
    sys.exit(linter.lint(code))

if __name__ == "__main__":
    main()