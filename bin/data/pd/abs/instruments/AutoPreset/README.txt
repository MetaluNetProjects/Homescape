#AutoPreset abstractions

Simple state saving for PureData

--------------------------------

###requirements :
You need following externals :

-	moonlib
-	iemlib
-	ggee


--------------------------------

###help :
Open `0autopreset-help.pd` for help and examples.

AutoPreset is a simple state saving system (like rradical/ssad, among other ones). It aims to be simple and yet powerful.

One particularity of this system is to be able to interpolate float parameters between different settings (parameter morphing).

There are 3 main objects :

-	apmaster : define a master Reference ; save and load preset files.
-	apsubmaster : define a sub-Reference, under another Reference.
-	apf (float), apsym (symbol), aptab (table) (...) : parameter types, which must depend on a Reference.


--------------------------------
	Antoine Rousseau 2004-2015
license : GNU GPL (see [LICENSE.txt](LICENSE.txt)) 
get last version there: <https://github.com/MetaluNet/AutoPreset>

