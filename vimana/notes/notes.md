NOTES IN SWEDISH
================

2021-04-25:

VERSION 1:

(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 TIMESDO
4.33s user 0.02s system 99% cpu 4.353 total

VERSION 2:

(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP
5.37s user 0.02s system 99% cpu 5.397 total

VERSION 3:

(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP
14.93s user 0.03s system 99% cpu 14.972 total

Hejsan, har fixat med min interpretator. Har tre versioner, en "basic" som använder PHP-stacken, en med svansrekursion men fortfarande PHP-stacken för "vanlig" rekursion, och en med explicit callstack. Det funkar hyffsat bra!

Prestanda är som följer:

Version 1:
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 TIMESDO
4.33s user 0.02s system 99% cpu 4.353 total

Version 2:
(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP
5.37s user 0.02s system 99% cpu 5.397 total

Version 3:
(L N LOOP) (N 0 EQ NOT (L DO L N 1 - LOOP) IFTRUE) DEF
(N FACT) (N 0 EQ (1) (N 1 - FACT N *) IFELSE) DEF
(20 FACT) 100000 LOOP
14.93s user 0.03s system 99% cpu 14.972 total

Version 3 med callstacken tog 24 sekunder förut. Det finns mycket att vinna på att optimera PHP-koden. Funktionsanrop till exempel tar tid, så man får "inlina" kod och undvika att abstrahera så mycket och skriva långa funktioner istället för många mindre. Tabeller där uppslagning sker (PHP associativa arrayer) verkar snabba och är framförallt snabba när man kopierar dom ("copy on write"). Det gör att uppslagningar verkar gå hyffsat fort. 

Min plan nu är att göra en mer optimerad version av version 3. Jag undrar om det är någon generell "regel" att det är snabbare att använda implementationsspråkets callstack? Chicken Scheme gör ju det till exempel, vad jag förstår. Men det blir roligare med en egen callstack för man kan göra diverse hyss och sen även göra parallella processer tänker jag.

Ursäkta excessen i uppercase, det är gammal nostalgi. Man kan ju göra som man vill själv genom att modda primitiven. Det är meningen att det ska vara lätt att lägga till primitiv, och primitiven är helt fristående från interpretatorn. Kör man utan primitiv hamnar alla element i programmet på datastacken! Själva interpretatorn och parsern är ca 160 rader för v1, ca 215 för v2 och ca 290 för v3. Sen är nuvarande primitiv runt 350 rader men det kan man ju så klart utveckla mycket mer.

2021-05-08:

Hallå i stugan! Hur har du det? Har kodat vidare, två veckor senare är C-versionen av interpretatorn "färdig". Den ha en explicit callstack som stöder svansrekursion. Jag använder inga bibliotek (förutom standard clib). Har implementerat en list-typ som bygger på en dynamisk array som utgör grunden. Alla uppslagningar sker med index (inga hashmappar). Koden är ca 1500 rader allt som allt.
Som prestandatest har jag använt fakultet 20. Första versionen
I första versionen tog 100.000 iterationer 2.5 sek. Det var något av en besvikelse, för den snabbaste PHP-versionen jag hade tog 4.3 sek. Så jag satte igång att optimera, med macron, återanvändning av stackframes osv. Kom ner till 1 sek. Sen kom jag på att man kunde slå på kompilatoroptimering och då blev det ungefär 0.4 sek.  Ytterligare optimering och det blev ännu lite snabbare ca 0.27 sek.
Mitt mål var att om möjligt klå Python, men nådde inte riktigt dit. Så här ser testresultaten ut, totalt sett:

C:            0.84s
PHP:          6.14s
Python:      21.38s
Vimana PHP: 433.00s (optimized version)
Vimana C:   254.00s (first version)
Vimana C:    26.61s (optimized version)
Men lite till optimering kanske interpretatorn blir ungefär lika snabb som Python. Intressant att är PHP är mycket snabbare än jag väntade mig. Och Python långsammare än jag väntade mig.
Jag interpreterar syntaxträdet direkt (som i Lisp), så för ytterligare uppsnabbningar får man väl använda bytekod eller liknande. Men jag tror jag nöjer mig så här.

Jag har brutit mot i princip varenda regel som finns, allt ifrån kodstil, filstruktur, till licensiering. Halva nöjet är att vara en rebell. Jag ser det som att jag bygger en modelljärnväg på vinden.

2021-05-09:

Hejsan, har nu svängt ihop en JavaScript-implemention av interpretatorn. Det är bra prestanda på JavaScript, 4.25s for 100.000 iterationer 20 FACT, och som jämförelse samma i "native" JavaScript 2.12s för 10.000.000 iterationer fact(20). Det är väldigt snabbt. I webbläsaren dessutom! (Kör Vivaldi). C presterade 0.4s tror jag det handlade om.

2021-05-26:

Jag har fått bättre prestanda på C-versionen, ca 13 sekunder för 10.000.000 factorial(20). Python gör samma benchmark på 21 sekunder, PHP 6 sekunder,  JS 2 sekunder och C 0.4 sekunder. Har gjort någon form av rudimentär GC med referensräkning, men inte testat den särskilt mycket. Pysslar mest med stilistiska detaljer i språket.

