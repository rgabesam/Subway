



# Subway - Optimalizace jízdních řádů pomocí diskrétní simulace



## Zadání

Cílem programu *Subway* je na základě vstupních dat, které se skládají z linek dopravy, frekvence jednotlivých stanic, vzdálenosti mezi nimi, počtu cestujících apod., optimalizovat jízdní řád tak, aby byl potenciál dopravního prostředku ideálně využit.



## Výroba ideálního jízdního řádu - algoritmus

Celá doba provozu je v programu rozplánovaná do sekcí. Na začátku je pro každou linku pouze jedna sekce. Sekce jednotlivých linek jsou nezávislé. Každá sekce má uvnitř nastaveno v jakých intervalech se budou v dané sekci vypouštět vlaky na trať. Simulace pak probíhá popořadě podle všech sekcí. Každý vypuštěný vlak si zapamatuje v jaké sekci byl vypuštěn na trať a pak pravidelně aktualizuje potenciál dané sekce, avšak sekce si vždy zapamatuje maximum. Tudíž na začátku simulace každá sekce začíná s potenciálem = 0 a velkým intervalem a v průběhu simulace si zapamatuje maximální potenciál ze všech vlaků vypuštěných v dané sekci.

> potenciál = počet cestujících ve vlaku / kapacita vlaku, 	 potenciál ∈ <0,1>

Po doběhnutí simulace jednoho dne se projdou všechny sekce a zkontroluje se, zda-li jsou potenciály sekcí v mezích daných uživatelem.  Podle toho zda je nebo není potenciál v rámci mezí se upraví interval vlaků v dané sekci a spustí se další simulace. Každý stav má svůj vnitřní stav, který odpovídá stavu ve stavovém automatu (viz obrázek), který znázorňuje úpravu intervalů.

![alg](alg.jpg)

Součástí vstupu je i přirozené číslo určující přesnost. Toto číslo určuje kolikrát každá sekce může navštívit stav 0.  Ve chvíli, kdy jsou všechny sekce ve stavu 4 se všechny sekce, které jsou větší než dvojnásobek svého intervalu, rozdělí napolovic. Všechny které toto kritérium nesplňují zůstanou stejné až na ty které skončily ve stavu 4, s potenciálem nižším než spodní hranice. Takové sekce se spojí s jednou následující aby se v nich mohl zvětšovat interval. Interval totiž nesmí přesáhnout velikost sekce.

Tento cyklus se stále opakuje dokud u všech linek se nezmění ani jedna sekce. Pak algoritmus skončí a vydá nový soubor s ideálními intervaly.
![alg2](alg2.jpg) 

## Simulace dne 

![alg3](alg3.jpg)