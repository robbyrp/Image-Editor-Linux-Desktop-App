<!--Copyright 2024 Darius Neatu (neatudarius@gmail.com)-->
# Checker PCLP1 - teme

Acesta este README-ul pentru checkerul de la tema 3 la materia PCLP1, seria CA.
Vă rugăm să descărcați arhiva cu checkerul local la fiecare temă și la fiecare actualizare a acestuia.

- [Checker PCLP1 - teme](#checker-pclp1---teme)
  - [Instalare](#instalare)
  - [Utilizare](#utilizare)
  - [Erori uzuale](#erori-uzuale)
  - [Activare verificare cu valgrind](#activare-verificare-cu-valgrind)
  - [Ierarhie teste](#ierarhie-teste)

## Instalare

* Bash (versiunea default  de pe OS) și Python 3 (>= 3.6) deja instalate pe sistem. Pe Ubuntu 24 ar trebui să aveți deja această parte configurată:
```bash
dariusn@pc:~$ python3 --version
Python 3.12.3

dariusn@pc:~$ bash --version
GNU bash, version 5.2.21(1)-release (aarch64-unknown-linux-gnu)
```
* Un exemplu de instalare pe Ubuntu:
```bash
$ sudo ./install.sh
```
Dacă scriptul eșuează, încercați să instalați manual dependențele prezente în acesta (gcc, make, pachete de Python).

## Utilizare

În funcție de cum aveți sistemul configurat, puteți rula checkerul pentru a vedea pagina de help, în unul din
următorele 2 moduri.

```bash
# Varianta 1 de rulare:
$ ./check --help
# Varianta 2 de rulare:
$ python3 ./check --help

usage: check [-h] [--task {image_editor}] [--legend {True,False}]

check homework

options:
  -h, --help            show this help message and exit
  --task {image_editor}
                        task name
  --legend {True,False}
                        print legend
```

Checkerul testează pas cu pas, tema curentă. De exemplu, trebuie să aveți un fișier `Makefile` și sursele să aibă numele specifica în enunț. În caz de eroare, se va opri opri ca în exemplu cu eroare `Makefile is missing!`:
```bash
$ python3 ./check
+++++ check_deps ...
system               - Linux
	 config: Darwin MacBook-Pro.local 23.6.0 Darwin Kernel Version 23.6.0: Fri Jul  5 17:53:24 PDT 2024; root:xnu-10063.141.1~2/RELEASE_ARM64_T6020 arm64
gcc                  - installed
	version: Apple clang version 15.0.0 (clang-1500.1.0.2.5)
make                 - installed
	version: GNU Make 3.81
python3              - installed
	version: Python 3.9.6
----- check_deps

+++++ make_build ...
Makefile is missing!

			Final grade: 0
TOTAL: 0/100


Total execution time: 0:00:00.061070
```

## Erori uzuale


* Citiți cu atenție cerințele din enunțuri și regulamentul temei curente. De exemplu, numele fișierelor Makefile, README, surselor, executabilelor etc.
* Citiți cu atenție outputul de la checker, linie cu linie. Dacă acesta doreste să instalați ceva, va printa un mesaj.

Erori la `build`: În această etapă se încearcă compilarea codului:
* 2.1. Fișierul `Makefile`: Trebuie să fie prezent.
* 2.2. Comanda `make`:  `make build` trebuie să compileze fișierele ca în enunț.
* 2.3. `warnings`:  Se aplică depunctări automate, ca în enunț.

Erori la `run`: În această etapă se încearcă rularea executabilelor:
* 3.1 `run`: Se rulează executabilul pe un singur test.
* 3.2 `check`: Dacă `run` a fost cu success (programul a ieșit cu cod 0), se verifică outputul produs.
* 3.3 `valgrind`: La temele cu alocare dinamică, se verifică încă o dată pentru memory leaks. Se poate suprascrie punctajul de la etapa `check`.

Legendă cu erori semnalate de checker:
1. `UPS`: Ups, programul a crăpat!
      Programul a ieșit cu cod diferit de 0! Checkerul nu poate detecta în mod automat eroarea, dar vă recomandăm să rulați manual testul respectiv. Exemple posibile: recursivitate infinită, acces invalid la memorie, dereferențiere de pointeri invalizi etc.
2. `TLE`: Time Limit Exceed (timp de executie depășit)
De obicei, acest lucru înseamnă ciclu infinit, deoarece la PCLP nu avem constrângeri de performanță. Cu toate acestea, la unele probleme se pot cere explicit în enunț soluții mai eficiente.
1. `MLE`: Memory Limit Exceed (limită de memorie depășită)
De obicei, acest lucru înseamnă că ați alocat prea multe tablouri sau aveți prea multe apeluri recursive, deoarece la PCLP nu avem constrângeri de memorie exacte. Cu toate acestea, la unele probleme se pot cere explicit în enunț soluții mai eficiente.
1. `MEM_UPS`: Memory leaks or errors
După rularea cu valgrind, checkerul a găsit memory leaks, accesse invalide la memorie, eliberări duble de memorie, utilizări de zone de memorie după ce acestea au fost eliberate etc. Vă recomandăm să rulați manual testul respectiv cu valgrind.
1. `WA`: Wrong Answer (wrong or partial output)
Programul s-a terminat cu success (cod 0), dar nu a produs outputul cerut (total sau parțial.)
1. `OK`: Viața e frumoasă
      Pe un test marcat OK, programul se termină cu succes (cod 0),
      nu are erori de timp sau memorie detectate în mod automat, iar outputul produs este identic cu cel cerut pentru respectivul test.

Erori de `style`: Se va rula automat un checker de coding style, care va raporta toate erorile pe care le aveți, conform https://ocw.cs.pub.ro/courses/programare/coding-style. Acesta va acorda puncte bonus (proporțional cu punctajul pe teste) sau penalizări.


Erori la `README`: Checkerul NU poate verifica calitatea fișierului README, însă vă ajută să nu uitați să adaugați totuși un README în rezolvare.

Erori la `clean`: `make clean` ar trebui să curețe fișierele produse la build.

`grade`: Reprezintă nota/punctajul pe care îl primiți pe o anumită categorie.

## Activare verificare cu valgrind

Din rațiuni ce țin de durată a testării, pe vmchecker **NU** vom testa cu valgrind. Temele vor fi testate cu valgrind separat de către asistenți, iar în cazul în care acestea prezintă memory-leaks sau accese invalide de memorie (detectate de valgrind sau prin analiza codului) acestea vor fi depunctate cu **20% din punctajul total al temei**.

Puteți verifica în mod automat același lucru la voi pe sistem înainte de a submite tema.

Pentru a activa testarea automată cu valgrind a `tuturor testelor`, setați câmpul `valgrind: true` în `config.json`.

Exemplu:

```json
[...]
 "name": "image_editor",
            "points": 90,
            "tests": 69,
            "timeout": 180,
            "stdin": true,
            "stdout": true,
            "valgrind": true,
            "points_distribution": ...
[...]
```

Pentru a activa testarea automată cu valgrind a `anumitor teste`, setați câmpul `tests_valgrind: [ ... ]` în `config.json`.

Exemplu:

```json
[...]
 "name": "image_editor",
            "points": 90,
            "tests": 69,
            "timeout": 180,
            "stdin": true,
            "stdout": true,
            "tests_valgrind": [0, 10],
[...]
```


## Ierarhie teste

Input-ul pentru teste se găsește în folder-ul `tasks/image_editor/tests`

Pentru folosirea manuală a testelor, test cu test, puteți să accesați testele cu fișierele de intrare (*.in) și cele de referință (*.ref - care conțin răspunsurile corecte), din folderul `tasks/image_editor/tests`.

Dacă dorim să inspectăm ce output / eroare a produs programul nostru pe testul `10`, putem să ne uităm la următoarele fișiere:

```bash
$ ls -lah tasks/image_editor/tests/10-image_editor/
total 20K
[...]
-rw-rw-r--  1 dariusn staff  412 dec  6 12:24 10-image_editor.in # Datele de intrare pentru testul 10.
-rw-rw-r--  1 dariusn staff  424 dec  6 13:15 10-image_editor.ref # Datele de iesire corecte pentru testul 10.
-rw-rw-r--  1 dariusn staff  424 dec  6 12:24 10-image_editor.out # Datele de iesire produse de programul nostru pentru testul 10.
-rw-rw-r--  1 dariusn staff    0 dec  6 13:15 10-image_editor.error # Datele de eroare produse de programul nostru pentru testul 10 (STDERR).
```

Pe lânga aceste fișiere, checker-ul va testa și corectitudinea transformărilor aplicate asupra imaginilor.

Imaginile de output și imaginile de referință pentru un anumit test se pot găsi în fișierul `tests.json`.
De exemplu, pentru testul 10, avem următoarea structură:

```json
"10-image_editor": {
        "files": [
            {
                "output": "output/ss_s_ba_1a.ppm",
                "ref": "ref/ss_s_ba_1a.ppm",
                "type": "ascii"
            },
            {
                "output": "output/ss_s_ba_2a.ppm",
                "ref": "ref/ss_s_ba_2a.ppm",
                "type": "ascii"
            },
            {
                "output": "output/ss_s_ba_3a.ppm",
                "ref": "ref/ss_s_ba_3a.ppm",
                "type": "ascii"
            },
            {
                "output": "output/ss_s_ba_4a.ppm",
                "ref": "ref/ss_s_ba_4a.ppm",
                "type": "ascii"
            },
            {
                "output": "output/ss_s_ba_5a.ppm",
                "ref": "ref/ss_s_ba_5a.ppm",
                "type": "ascii"
            }
        ],
        "verbose_name": "simple_select_apply_gaussianblur_ba"
    },
```

Fiecare fișier de input va produce un număr de fișiere de output / imagini prin intermediul comenzilor de `SAVE`.

Pentru un fișier de output, distingem trei câmpuri:

1. `output` -  numele fișierului generat de programul nostru în urma rulării unui test
1. `ref` - numele fișierului de referință în urma rulării unui test
1. `type` - tipul fișierului, poate avea una din valorile `ascii / binary`

Pentru fiecare dintre fișierele din secțiunea `files` asociate unui anumit test, se va rula utilitarul `image_check` pentru a verifica dacă imaginile corespund.
Punctajul pentru fiecare fișier de input este binar astfel:

- punctajul maxim pe test în cazul în care fișierul de `xy-image_editor.out` coincide cu `xy-image_editor.ref` (unde xy este numărul testului) și rularea utilitarului de `image_check` pe imaginile produse de program nu identifică diferențe la compararea cu imaginile de referință
- punctajul nul pe test în orice alt caz

---

Puteți ignora parametrul `verbose_name`, el este folosit în mod intern de checker pentru verificări.

---

Dacă dorim să folosim un fișier de test pentru o rulare manuală în afara checkerului, putem folosi redirectări:

```bash
# Compilare manuala.
$ make

# Rulare manuala test 00: citire input cu redirectare din fisier, afisare la STDOUT a rezultatului.
$  ./image_editor < tasks/image_editor/tests/10-image_editor/10-image_editor.in

# Rulare manuala test 00: citire input cu redirectare din fisier, afisare cu redirectare output in fisierul hai_steaua.out
$  ./image_editor < tasks/image_editor/tests/10-image_editor/10-image_editor.in  > hai_steaua.out

# Validare că imaginile generate de programul nostru corespund cu imagile de referință
./image_check tests.json 10-image_editor
```

Dacă există orice inconsistență, executabilul image_check va afișa un mesaj de eroare și va ieși cu un cod de eroare:

* "<name> was not found" - fișierul de output nu a fost găsit
* "<name> ref is missing" - fișierul de referință nu există în directorul `ref`
* "The images are not the same type" - imaginea generată nu are același tip cu imaginea de referință
* "File <name> has a differnce more than <EPS> (<diff>) at <x>, <y> - pentru imaginea generată există pixelul de la poziția (x, y) există o diferență de `diff` față de valoarea pentru același pixel din imaginea de referință ce este mai mare decât o toleranță EPS


