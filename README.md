# BigInt

## 1. Overview e funzionalità

`BigInt` è una classe C++ che rappresenta interi con un numero arbitrario di cifre decimali, superando i limiti dei tipi built-in come `int`.

La classe supporta:

- costruttore di default, da tipi interi signed/unsigned fino a `long long`/`unsigned long long` e da `std::string`;
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
- valori limite signed e unsigned, inclusi `INT_MIN`, `LLONG_MIN` e `ULLONG_MAX`;
- carry, borrow e numeri con segni diversi;
- divisione, modulo e divisione per zero;
- operatori binari e compound;
- confronti;
- pre/post incremento e decremento;
- shift e bitwise, anche su numeri negativi;
- copy semantics, self-assignment e move con sorgente riportata a zero;
- operatori binari con `int` o `unsigned int` anche a sinistra;
- `pow`, stream output e input invalido.

## 3. Benchmark indicativo

`BigIntBenchmark.cpp` misura alcune operazioni su numeri di 16, 64 e 256 cifre decimali.

Per ripetere la misura dalla cartella del progetto:

```bash
clang++ -std=c++17 -O2 -Wall -Wextra -Wpedantic \
    BigIntBenchmark.cpp BigInt.cpp BigIntArithmetic.cpp BigIntComparison.cpp \
    BigIntBitwise.cpp BigIntIO.cpp -o bigint-benchmark
./bigint-benchmark
```

Risultati raccolti su macOS 26.6.2, arm64, Apple clang 17.0.0 con `-O2`. Ogni valore è la mediana di 7 esecuzioni del programma. Il tempo per operazione è il tempo totale diviso per il numero di ripetizioni.

| Operandi | Operazione | Ripetizioni per esecuzione | Mediana totale | Tempo per operazione |
| --- | --- | ---: | ---: | ---: |
| 16 cifre | Somma | 20.000 | 2,449 ms | 0,122 µs |
| 16 cifre | Moltiplicazione | 2.000 | 1,232 ms | 0,616 µs |
| 16 cifre | Divisione | 2.000 | 14,973 ms | 7,487 µs |
| 64 cifre | Somma | 5.000 | 0,977 ms | 0,195 µs |
| 64 cifre | Moltiplicazione | 300 | 3,352 ms | 11,173 µs |
| 64 cifre | Divisione | 500 | 22,243 ms | 44,486 µs |
| 256 cifre | Somma | 1.000 | 0,427 ms | 0,427 µs |
| 256 cifre | Moltiplicazione | 30 | 6,152 ms | 205,067 µs |
| 256 cifre | Divisione | 100 | 44,752 ms | 447,520 µs |

Per `pow(2, 512)`, 20 ripetizioni hanno richiesto una mediana di 2,333 ms, pari a circa 116,650 µs per operazione.

Gli operandi vengono costruiti prima della misura. Il ciclo misura operazione e assegnazione del risultato; la conversione finale in testo è fuori dal tempo misurato. Gli stessi operandi sono riutilizzati in ogni ripetizione. I risultati sono indicativi per questa macchina e questi input: non sono un confronto con altre librerie o rappresentazioni, né misurano la memoria usata.

## 4. Rappresentazione e invarianti

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

`BigInt` possiede il proprio `std::vector<int>`, che gestisce automaticamente lo storage dinamico. Le operazioni di copia sono quelle di default; move constructor e move assignment sono espliciti per lasciare l'oggetto sorgente nello stato canonico `0`. Questa garanzia può richiedere un'allocazione e quindi le operazioni di move non sono dichiarate `noexcept`.

## 5. Scelte progettuali e trade-off

La scelta di usare una cifra decimale per elemento rende immediati parsing e stampa, oltre a semplificare il ragionamento durante l’implementazione.

L’ordine little-endian è utile per addizione e sottrazione: carry e borrow partono dall’indice `0`, mentre un’eventuale cifra finale viene aggiunta con `push_back`.

Gli operatori compound modificano `*this` e restituiscono `BigInt&`; gli operatori binari sono funzioni non membro che ricevono l'operando sinistro per valore, applicano il corrispondente operatore compound e restituiscono il risultato. Questo permette la conversione implicita di interi built-in in `BigInt` su entrambi i lati, senza modificare gli operandi originali.

La divisione segue la semantica di troncamento verso zero. Il resto mantiene il segno del dividendo.

Gli shift sinistri corrispondono a moltiplicazioni per potenze di due. Gli shift destri sono definiti come shift aritmetici: per i numeri negativi arrotondano verso `-∞`.

Le operazioni bitwise su numeri negativi usano internamente una rappresentazione temporanea in complemento a due a larghezza sufficiente.

`pow` utilizza l’esponenziazione veloce: dimezza l’esponente e quadra la base a ogni iterazione, riducendo il numero di moltiplicazioni rispetto a un ciclo lineare.

## 6. Difficoltà progettuali e soluzioni adottate

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

## 7. Limiti e possibili evoluzioni

La rappresentazione corrente privilegia chiarezza e semplicità rispetto alle prestazioni su numeri molto grandi.

Possibili evoluzioni:

- sostituire `std::vector<int>` con chunk `std::uint32_t`, usando una base più grande per ridurre memoria e numero di iterazioni;
- usare un tipo temporaneo più ampio, ad esempio `std::uint64_t`, per prodotti e carry tra chunk;
- confrontare con un benchmark la rappresentazione per cifre e una futura rappresentazione a chunk;
