# AST Project Readme
Lucas Lopilato


For this project, there were two hard parts, fully understanding the AST framework
and the lexer bug that I had in my code.

Once I understood the framework the solution was relatively simple.
Most of my grammar was already implemented to be left recursive, so whenever
a %empty rule was run, I would have my grammar create a new list of
the respective type. For rules that used a recursive call, I would just 
push the type onto the list.

I was also able to find a few grammar bugs via the classes as well. 
The class definitions in ast.hpp described exactly how your project was
created so I could infer and piece together the grammar.

It also helped with string initializations and string usage within the grammar.


The most difficult challenge was a print statement that ended up coming from
the lexer saying error--no action found. Using gdb, I found that it came from 
when the ast class went to parse through.

Where it actually came from is when ast flushes stdout, which contained errors
from lexer.l. From there, I went through test cases where it occurred and
singled out statements that caused it. I then backtracked and found that my 
lexer improperly handled scanning characters and strings. Once I located it
I made minor changes and had no problems
