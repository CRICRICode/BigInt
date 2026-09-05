# BigInt

## 1. Overview e funzionalità

`BigInt` è una classe C++ che rappresenta interi con un numero arbitrario di cifre decimali, superando i limiti dei tipi built-in come `int`.

La classe supporta:

- costruttore di default, da `int` e da `std::string`;
- parsing di segni `+` e `-`, inclusi zeri ridondanti;
- operatori aritmetici `+`, `-`, `*`, `/`, `%` e relative versioni compound;
- confronti `==`, `!=`, `<`, `<=`, `>`, `>=`;
- incremento e decremento pre/post;
- negazione unaria `-` e bitwise NOT `~`;
- operatori bitwise `&`, `|`, `^` e versioni compound;
- shift `<<` e `>>`, incluse versioni compound;
- `pow` con esponente `int` o `BigInt`;
- output tramite `std::ostream` e input tramite `std::istream`.

Gli input testuali non validi nel costruttore lanciano `std::invalid_argument`.  
L’estrazione da stream imposta `failbit` e mantiene invariato il valore di destinazione se il testo letto non è un `BigInt` valido.

## 2. Compilazione e test

Il progetto richiede un compilatore con supporto C++17.

Dalla cartella del progetto:

```bash
g++ -std=c++17 -Wall -Wextra -Wpedantic \
    main.cpp BigInt.cpp BigIntArithmetic.cpp BigIntComparison.cpp \
    BigIntBitwise.cpp BigIntIO.cpp -o main && ./main
```

`main.cpp` contiene una test suite con un piccolo test harness personalizzato.  
Il programma restituisce `0` se tutti i test passano e `1` se almeno un test fallisce.

La suite verifica, tra gli altri:

- costruttori, parsing e normalizzazione;
- `INT_MIN`;
- carry, borrow e numeri con segni diversi;
- divisione, modulo e divisione per zero;
- operatori binari e compound;
- confronti;
- pre/post incremento e decremento;
- shift e bitwise, anche su numeri negativi;
- copy semantics, self-assignment e move;
- `pow`, stream output e input invalido.

## 3. Rappresentazione e invarianti

Il numero è rappresentato tramite:

```cpp
std::vector<int> digits;
bool negative;
```

`digits` contiene cifre decimali in ordine little-endian: la cifra meno significativa è in posizione `0`.

Esempio:

```text
-2300 → digits = {0, 0, 3, 2}, negative = true
```

Gli invarianti della classe sono:

- `digits` non è mai vuoto;
- ogni elemento di `digits` è compreso fra `0` e `9`;
- non esistono zeri ridondanti nella parte più significativa;
- lo zero è rappresentato sempre come `{0}` con `negative == false`;
- `-0` non è mai uno stato valido.

`BigInt` possiede il proprio `std::vector<int>`, che gestisce automaticamente lo storage dinamico. La classe applica quindi la Rule of 0: non implementa manualmente distruttore, copy constructor o move constructor.

## 4. Scelte progettuali e trade-off

La scelta di usare una cifra decimale per elemento rende immediati parsing e stampa, oltre a semplificare il ragionamento durante l’implementazione.

L’ordine little-endian è utile per addizione e sottrazione: carry e borrow partono dall’indice `0`, mentre un’eventuale cifra finale viene aggiunta con `push_back`.

Gli operatori compound modificano `*this` e restituiscono `BigInt&`; gli operatori binari lavorano invece su una copia e restituiscono un nuovo `BigInt`, preservando il valore dell’operando sinistro.

La divisione segue la semantica di troncamento verso zero. Il resto mantiene il segno del dividendo.

Gli shift sinistri corrispondono a moltiplicazioni per potenze di due. Gli shift destri sono definiti come shift aritmetici: per i numeri negativi arrotondano verso `-∞`.

Le operazioni bitwise su numeri negativi usano internamente una rappresentazione temporanea in complemento a due a larghezza sufficiente.

`pow` utilizza l’esponenziazione veloce: dimezza l’esponente e quadra la base a ogni iterazione, riducendo il numero di moltiplicazioni rispetto a un ciclo lineare.

## 5. Difficoltà progettuali e soluzioni adottate

Le principali difficoltà affrontate sono state:

- mantenere una rappresentazione canonica dello zero;
- separare il segno dalla magnitudine;
- gestire carry e borrow con numeri di lunghezza diversa;
- implementare divisione e modulo con segni diversi;
- distinguere operatori mutanti e non mutanti;
- gestire correttamente `INT_MIN` nel costruttore da `int`;
- definire una semantica esplicita per gli shift dei negativi;
- implementare le operazioni bitwise tramite conversione temporanea in bit e complemento a due;
- mantenere l’input da stream consistente: un input invalido non modifica l’oggetto di destinazione.

La normalizzazione centralizza il mantenimento degli invarianti dopo le operazioni che possono produrre zeri ridondanti o un possibile `-0`.

## 6. Limiti e possibili evoluzioni

La rappresentazione corrente privilegia chiarezza e semplicità rispetto alle prestazioni su numeri molto grandi.

Possibili evoluzioni:

- sostituire `std::vector<int>` con chunk `std::uint32_t`, usando una base più grande per ridurre memoria e numero di iterazioni;
- usare un tipo temporaneo più ampio, ad esempio `std::uint64_t`, per prodotti e carry tra chunk;
- migliorare ulteriormente divisione e modulo con algoritmi più efficienti;
- separare la test suite da `main.cpp` e adottare un framework dedicato;
- aggiungere benchmark per confrontare rappresentazione per cifre e rappresentazione a chunk;
- ampliare la gestione dell’input da stream, ad esempio con messaggi diagnostici esterni o policy di parsing configurabili.

La rappresentazione a chunk sarebbe particolarmente utile in un contesto orientato a performance, ma richiederebbe un refactor completo di parsing, stampa, normalizzazione e operazioni aritmetiche.
