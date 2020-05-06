# PS-Projekt

root pw: s  
```
sudo apt-get update  
sudo apt-get install git  
git clone https://github.com/matusino221/PS-Projekt.git  
git pull  
```
commit:  
```
git add .  
git commit -m "sprava"  
git push  

#TEMA:  

4. Simulácia bezdrôtovej siete v poli.  
– Nastavte vhodnú fyzickú interpretáciu prostredia pomocou modelov definovaných v dokumentácii NS3.  
— Hospodár sa pohybuje v okolí, chvíľu je a potom nie je v dosahu siete. Zvoľte vhodný model pohybu/pohybov.  
– Vyberte a nastavte vhodný MAC protokol (WifiManager), upravte nastavenia.  
– Vyberte a nastavte vhodný/é smerovací/ie protokol/y, upravte nastavenia.  
– Vyberte a nastavte vhodný/é transportný/é protokol/y, upravte nastavenia.  
– Aplikačná časť:  
— senzorová sieť monitoruje prostredie. Poľnohospodár pritom občas príde a chce stiahnuť informácie zo senzorovej siete. Monitorujte stav batérie.  
— simulujte pre každý senzor batériu s energiou (može byť aj s dobíjaním). Môžete využiť model v NS3 alebo použiť vlastný objekt, kde si môžete zaznamenávať stavy o počte komunikácii, zapnutí …  
– príklad na udalosť: Vstup poľnohospodár do dosahu senzorovej siete, Pri nízkom stavu batérie sa zmení smerovacia tabuľka, a pod.  

Na vizualizáciu simulácie využite program a modul NetAnim. Zadefinujte aspoň 2 merané parametre QoS, ktoré vynesiete do grafov za pomoci modulu a programu Gnuplot. Simuláciu spustite viac krát, meňte pomocou SeedManager nastavenia generovania náhodnej premennej, aby ste získali štandardnú odchýlku merania (Errorbars) pre vynesené body grafu. Vhodne stanovte čas-trvanie simulácie.  

Príklad QoS (pomer priemerného počtu poslaných užitočných údajov k celkovému počtu poslaných údajov na jednu cestu/balík), príklad parametra počet vedľajších komunikácii v sieti (údržba siete, pripojenie/odpojenie, hľadanie cesty, …).  
