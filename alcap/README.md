alcap
=====

Workding directory for Alcap simulation

#Interactive Mode Tutorial
*To Be Added*

#Batch Mode Tutorial
*To Be Added*

#Using Job Control Scripts
*To Be Added*

#Specific Simulations
1.	**For Gemanium Detector Callibration**  
	1.	Generate **Eu152** to find peaks and check the ratio of each peak 

				cd $MYG4SIMWORKROOT
				g4sim macros/CheckEu152.mac
				cd scripts/GeCallibration
				root -l get_peaks.C

		You will get:  

				Total events: 1000000
				Events with RadioactiveDecay Gamma Ray: 991841
				Peaks with ratio over 5%: (error +-0.025keV)
					Index	Energy		Ratio
							keV   		Ngam/Nevents
					0	121.775		0.282415
					1	244.675		0.076972
					2	344.275		0.265659
					3	778.925		0.129626
					4	964.075		0.14587
					5	1085.875		0.101973
					6	1112.075		0.136631
					7	1407.975		0.210716
				Gammas from all peaks: 1594107
				Gammas from highest 8 peaks: 1349862

		Where the last number of each peak line is the ratio of that peak,
		which is defined as Ratio = N<sub>peak</sub>/N<sub>Eu152</sub>.

	2.	Generate **Eu152** from source position and record Ge response.
		Check the efficiency of each peak with given ratios (got from #1, written into `get_eff.C`)

				cd $MYG4SIMWORKROOT
				bg4sim_sub -t sim -b 0 -e 6 -Y GeCallibration GeCallibration001
				cd scripts/Callibration
				root -l get_eff.C

		You will get:  

				Total events: 7e+06
				Peak[0]: 121.775+-0.0205297% keV, Efficiency: 0.00140017
				Peak[1]: 244.675+-0.0102176% keV, Efficiency: 0.00105604
				Peak[2]: 344.275+-0.00726164% keV, Efficiency: 0.000757685
				Peak[3]: 778.925+-0.00320955% keV, Efficiency: 0.000416583
				Peak[4]: 964.075+-0.00259316% keV, Efficiency: 0.000315349
				Peak[5]: 1085.88+-0.00230229% keV, Efficiency: 0.000294196
				Peak[6]: 1112.08+-0.00224805% keV, Efficiency: 0.000295896
				Peak[7]: 1407.97+-0.0017756% keV, Efficiency: 0.0002061
				logE			logEff
				4.80218			-6.57116
				5.49993			-6.85323
				5.84144			-7.18524
				6.65791			-7.78342
				6.87117			-8.06183
				6.99014			-8.13127
				7.01398			-8.1255
				7.24991			-8.48715

1.	**For SiR2 Data Set**
	1.	Check the deposit energy and hit time distribution in SiR2
