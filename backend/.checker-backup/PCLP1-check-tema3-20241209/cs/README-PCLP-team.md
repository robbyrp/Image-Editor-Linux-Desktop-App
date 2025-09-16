# PCLP1 CA - Checker coding style


## Utilizare la teme

`cs.sh` verifică sursele primite ca și argument.
* Exit code `0` ȘI `absența` outputului înseamnă că nu au fost detectate probleme în mod automat.
* Altfel, au fost detectate probleme și vom depuncta la teme.

## Actualizare la ultima versiune de checkpatch.pl

### Actualizare script
```
# Download from https://github.com/torvalds/linux/blob/master/scripts.
mv checkpatch.pl > cs/checkpatch.pl

git add cs/checpatch.pl
git commit -m "checkpatch.pl: update to latest master version"

# !!! Don't forget to apply our internal patches
# e.g., checkpatch on master does not check the length of a function.
for patch in $(ls -v cs/patches); do git apply < cs/patches/${patch}; done
```

### Actualizare dependențe

* `const_structs.checkpatch` - trebuie să existe; îl păstrăm gol.
* `spelling.txt` - se trage de lângă checkpatch.pl.

## Testare

```bash
$ ./cs.sh tests/
tests/11-FUNCTION_SIGNATURE_MULTIPLE_LINES.c:2: CHECK:PARENTHESIS_ALIGNMENT: Alignment should match open parenthesis
tests/10-FUNCTION_TOO_LONG.c:168: WARNING:LONG_FUNCTION: 'func_not_ok1' function definition is 81 lines, perhaps refactor
tests/10-FUNCTION_TOO_LONG.c:271: WARNING:LONG_FUNCTION: 'func_not_ok2' function definition is 99 lines, perhaps refactor
tests/09-LINE_TOO_LONG.c:3: CHECK:LONG_LINE_STRING: line length of 91 exceeds 80 columns
tests/02-SUSPECT_CODE_INDENT.c:4: WARNING:SUSPECT_CODE_INDENT: suspect code indent for conditional statements (4, 4)
tests/03-FUNCTION_WITHOUT_ARGS.c:1: ERROR:FUNCTION_WITHOUT_ARGS: Bad function definition - void f() should probably be void f(void)
tests/03-FUNCTION_WITHOUT_ARGS.c:6: ERROR:FUNCTION_WITHOUT_ARGS: Bad function definition - int main() should probably be int main(void)
tests/08-EXTRA_WHITESPACE.c:3: WARNING:SPACING: space prohibited before semicolon
tests/08-EXTRA_WHITESPACE.c:4: WARNING:SPACING: space prohibited between function name and open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:4: ERROR:SPACING: space prohibited before that ',' (ctx:WxW)
tests/08-EXTRA_WHITESPACE.c:4: ERROR:SPACING: space prohibited before that close parenthesis ')'
tests/08-EXTRA_WHITESPACE.c:8: ERROR:SPACING: space required one side of that '++' (ctx:WxW)
tests/08-EXTRA_WHITESPACE.c:9: WARNING:SPACING: space prohibited before semicolon
tests/08-EXTRA_WHITESPACE.c:10: WARNING:SPACING: space prohibited between function name and open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:10: ERROR:SPACING: space prohibited before that ',' (ctx:WxW)
tests/08-EXTRA_WHITESPACE.c:10: ERROR:SPACING: space prohibited after that open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:10: ERROR:SPACING: space prohibited before that close parenthesis ')'
tests/08-EXTRA_WHITESPACE.c:11: WARNING:SPACING: space prohibited between function name and open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:11: ERROR:SPACING: space prohibited after that open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:11: ERROR:SPACING: space prohibited before that close parenthesis ')'
tests/08-EXTRA_WHITESPACE.c:19: ERROR:SPACING: space prohibited after that open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:19: ERROR:SPACING: space prohibited before that close parenthesis ')'
tests/08-EXTRA_WHITESPACE.c:20: WARNING:SPACING: space prohibited between function name and open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:20: WARNING:SPACING: space prohibited before semicolon
tests/08-EXTRA_WHITESPACE.c:20: ERROR:SPACING: space prohibited before that close parenthesis ')'
tests/08-EXTRA_WHITESPACE.c:21: WARNING:SPACING: space prohibited between function name and open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:21: WARNING:SPACING: space prohibited before semicolon
tests/08-EXTRA_WHITESPACE.c:21: ERROR:SPACING: space prohibited before that close parenthesis ')'
tests/08-EXTRA_WHITESPACE.c:25: WARNING:SPACING: space prohibited before semicolon
tests/08-EXTRA_WHITESPACE.c:25: ERROR:SPACING: space prohibited after that open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:26: WARNING:SPACING: space prohibited before semicolon
tests/08-EXTRA_WHITESPACE.c:26: ERROR:SPACING: space prohibited after that open parenthesis '('
tests/08-EXTRA_WHITESPACE.c:27: ERROR:SPACING: space prohibited before that close parenthesis ')'
tests/08-EXTRA_WHITESPACE.c:29: WARNING:SPACING: space prohibited before semicolon
tests/07-MISSING_WHITESPACE.c:4: ERROR:SPACING: space required after that ',' (ctx:VxO)
tests/07-MISSING_WHITESPACE.c:4: ERROR:SPACING: space required before that '&' (ctx:OxV)
tests/07-MISSING_WHITESPACE.c:6: ERROR:SPACING: spaces required around that '=' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:7: ERROR:SPACING: spaces required around that '=' (ctx:WxV)
tests/07-MISSING_WHITESPACE.c:8: ERROR:SPACING: spaces required around that '=' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:8: ERROR:SPACING: space required after that ';' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:8: ERROR:SPACING: spaces required around that '<' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:8: ERROR:SPACING: space required after that ';' (ctx:VxO)
tests/07-MISSING_WHITESPACE.c:8: ERROR:SPACING: space required before the open brace '{'
tests/07-MISSING_WHITESPACE.c:8: ERROR:SPACING: space required before the open parenthesis '('
tests/07-MISSING_WHITESPACE.c:12: ERROR:SPACING: spaces required around that '+=' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:13: ERROR:SPACING: spaces required around that '=' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:13: CHECK:SPACING: spaces preferred around that '*' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:16: ERROR:SPACING: spaces required around that '>' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:16: ERROR:SPACING: spaces required around that '&&' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:16: ERROR:SPACING: spaces required around that '>' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:17: ERROR:SPACING: spaces required around that '=' (ctx:VxV)
tests/07-MISSING_WHITESPACE.c:19: ERROR:SPACING: space required before the open brace '{'
tests/07-MISSING_WHITESPACE.c:19: ERROR:SPACING: space required before the open parenthesis '('
tests/07-MISSING_WHITESPACE.c:24: ERROR:SPACING: space required before the open brace '{'
tests/07-MISSING_WHITESPACE.c:27: ERROR:SPACING: space required after that close brace '}'
tests/07-MISSING_WHITESPACE.c:27: ERROR:SPACING: space required before the open parenthesis '('
tests/04-BLANK_LINES.c:5: CHECK:LINE_SPACING: Please use a blank line after function/struct/union/enum declarations
tests/04-BLANK_LINES.c:9: CHECK:LINE_SPACING: Please use a blank line after function/struct/union/enum declarations
tests/04-BLANK_LINES.c:12: CHECK:LINE_SPACING: Please don't use multiple blank lines
tests/04-BLANK_LINES.c:15: CHECK:LINE_SPACING: Please don't use multiple blank lines
tests/06-TRAILING_WHITESPACE.c:3: ERROR:TRAILING_WHITESPACE: trailing whitespace
tests/06-TRAILING_WHITESPACE.c:7: ERROR:TRAILING_WHITESPACE: trailing whitespace
tests/06-TRAILING_WHITESPACE.c:8: ERROR:TRAILING_WHITESPACE: trailing whitespace
tests/06-TRAILING_WHITESPACE.c:9: ERROR:TRAILING_WHITESPACE: trailing whitespace
tests/06-TRAILING_WHITESPACE.c:12: ERROR:TRAILING_WHITESPACE: trailing whitespace
```
