# HyperLisp!
The Lisp-to-HTML preprocessor

`usage: hyperlisp <input> <output>`

Add this to your README to show off:

```
[![built with HyperLisp!](https://img.shields.io/badge/built%20with-HyperLisp!-blue)](https://github.com/circl-lastname/HyperLisp)
```

[![built with HyperLisp!](https://img.shields.io/badge/built%20with-HyperLisp!-blue)](https://github.com/circl-lastname/HyperLisp)

## Installation
### Linux
With `ninja` and `gcc` installed
```
./configure
ninja
sudo ninja install
```
See `./configure --help` for more info on compilation.
### Windows
Executables are available in the [releases](https://github.com/circl-lastname/HyperLisp/releases).

## Quick start guide
[Try out the online demo!](https://circl-lastname.github.io/HyperLisp-demo)

```lisp
; A DOCTYPE is inserted autmatically, but if you don't want it you can use
(!nodoctype)

(html
  ; Elements are like functions in Lisp
  (p "I am a paragraph!")
  
  ; You can add attributes with keywords and strings or literals with a single quote, everything after them is the content of the element
  (a :href "https://example.com" :target '_blank "Very cool link!")
  
  ; Elements can be nested as ususal
  (p (img :src "verycoolimage.png")))
```

## Practical example
```lisp
(html :lang 'en
  (head
    (meta :charset 'utf-8)
    (title "circl's website"))
  
  (body
    (h1 "circl's website")
    
    (p "Welcome! I make software (usually of no practical use) as a hobby. This simple website serves as a hub for the stuff I do.")
  
    (h2 "Software projects")
    
    (ul
      (li (a :href "6502" "6502 stuff"))
      (li (a :href "mblf" "MBLF and Brainfuck stuff")))
    
    (h2 "Contact")
    
    (p "My Discord username is circl#3993, and my e-mail is " (a :href "mailto:circl.lastname@gmail.com" "circl.lastname@gmail.com") ".")))
```

## Known bugs and caveats
* You still have to escape `<` and `>` like in HTML.
* You always need to give content to attributes, even if they don't need it, empty strings work the same.
