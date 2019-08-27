    
Testele se execută cu comanda:

  make -f Makefile.checker

* Testele se rulează pe Windows în cygwin.


Pentru a inspecta diferențele între output-ul mini-shell-ului și cel al
binarului de referință setați 'DO_CLEANUP=yes' în '_test/run_test.sh'.

Pentru a inspecta resursele neeliberate (leak-uri de memorie, descriptori de
fișiere) setați 'USE_VALGRIND=yes' în '_test/run_test.sh'. Puteți modifica
atât calea fișierului de logging pentru valgrind, cât și parametrii comenzii.
Dacă variabila 'DO_CLEANUP' are valoarea 'yes', atunci se va genera câte un
fișier de log în directorul asociat fiecărui test.
