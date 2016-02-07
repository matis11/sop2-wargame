#include <stdio.h>
#include <stdlib.h>


void attack_p1(int lic, int hic, int cc){//atak ze strony p1 
	get_p2(int lic_p2,int hic_p2, int cc_p2);//pobieramy info o p2
	float p1a,p1d,p2a,p2d;//siła ataku/obrony p1/p2
	int tsl,tsh,tsc;//tymczasowe zmienne strat
		p1a=lic+(hic*1.5)+(cc*3.5);
		p1d=(lic*1.2)+(hic*3)+(cc*1.2);
		p2a=lic_p2+(hic_p2*1.5)+(cc_p2*3.5);
		p2d=(lic_p2*1.2)+(hic_p2*3)+(cc_p2*1.2);
		//siła ataku p1 vs siła obrony p2
		if(p1a>p2d){
			lic_p2=0;
			hic_p2=0;
			cc_p2=0;
		}else{
			tsl=lic_p2*p2d/p1a;
			tsh=hic_p2*p2d/p1a;
			tdc=cc_p2*p2d/p1a;
			lic_p2-=tsl;
			hic_p2-=tsh;
			cc_p2-=tsc;
		}
		tsl=lic*p1a/p2d;
		tsh=hic*p1a/p2d;
		tsc=cc*p1a/p2d;
		lic-=tsl;
		hic-=tsh;
		cc-=tsc;
		//siła obrony p1 vs siła ataku p2
		if(p2a>p1d){
			lic=0;
			hic=0;
			cc=0;
		}else{
			tsl=lic*p1d/p2a;
			tsh=hic*p1d/p2a;
			tsc=cc*p1d/p2a;
			lic-=tsl;
			hic-=tsh;
			cc-=tsc;
		}
		tsl=lic_p2*p2a/p1d;
		tsh=hic_p2*p2a/p1d;
		tsc=cc_p2*p2a/p1d;
		lic_p2-=tsl
		hic_p2-=tsh;
		cc_p2-=tsc;
	pushback(lic,hic,cc,lic_p2,hic_p2,cc_p2);
}

