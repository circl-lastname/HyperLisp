# HyperLisp
The Lisp-to-HTML preprocessor

`usage: hyperlisp <input> <output>`

## Known bugs and caveats
* If your input has an error, the error message may point you to the wrong character.
* You still have to escape `<` and `>` like in HTML.
* You always need to give content to attributes, even if they don't need it, empty strings work the same.

## Example
```lisp
(html :lang 'en
  (head 
    (meta :charset 'utf-8)
    (title "circl's website"))
  
  (body 
    (p (a :href ".." "Main page"))
    
    (h1 "6502 stuff")
    
    (p "The 6502 is a nice little chip. I forked an "
    (a :href "https://github.com/circl-lastname/6502js" :target '_blank "online 6502 simulator")
    ", and I'm messing around with it.")
    
    (p "I use "
    (a :href "http://sun.hasenbraten.de/vasm/" :target '_blank "vasm")
    " as my assembler, and a "
    (a :href "dcbify.js" :target '_blank "node script")
    " I made to convert the binary data to " (code "dcb") " statements for the online simulator.")))
```
