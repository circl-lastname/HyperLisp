# HyperLisp
The Lisp-to-HTML preprocessor

`usage: hyperlisp <input> <output>`
## Known bugs and caveats
* If your input has an error, the error message may point you to the wrong character.
* You still have to escape `<` and `>` like in HTML.
* You always need to give content to attributes, even if they don't need it, empty strings work the same.
