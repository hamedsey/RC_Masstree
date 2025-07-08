import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

tput_scq_100p = [890262
,1754638
,2315071
,2853446
,3377498
,3864884
,4149333
,4577818
,4792501
,5019797
,5217443]
lat_scq_100p  = [7728
,8169.6
,8537.6
,8905.6
,9494.4
,9862.4
,10304
,10745.6
,11334.4
,16707.2
,149113.6]
tput_scq_100p = [num / 1E6 for num in tput_scq_100p]
lat_scq_100p = [num / 1E3 for num in lat_scq_100p]

tput_scq_10p = [885816, 1763624
,2323178
,2875767
,3414608
,3669316
,3908352
,4158144
,4418812
,4625598
,4861496]
lat_scq_10p  = [5990, 6182.4
,6844.8
,7433.6
,8096
,8905.6
,10230.4
,12806.4
,13468.8
,1192688
,1329657.6]
tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
lat_scq_10p = [num / 1E3 for num in lat_scq_10p]
    
tput_sscq_100p = [883753, 1755754
,2860869
,3374873
,3619028
,3858064
,4099633
,4306725
,4559742
,4758895
,4988285
,5211240]
lat_sscq_100p  = [11365,12806.4
,21344
,22080
,22300.8
,22374.4
,22595.2
,22668.8
,23110.4
,23846.4
,26790.4
,124310.4]
tput_sscq_100p = [num / 1E6 for num in tput_sscq_100p]
lat_sscq_100p = [num / 1E3 for num in lat_sscq_100p]

tput_sscq_10p = [880521,1762990
,2324744
,2867715
,3658422
,3899264
,4147754
,4397186
,4619015
,4850509]
lat_sscq_10p  = [9742,10083.2
,13616
,16486.4
,18694.4
,20019.2
,22595.2
,1625750.4
,1402521.6
,1435862.4]
tput_sscq_10p = [num / 1E6 for num in tput_sscq_10p]
lat_sscq_10p = [num / 1E3 for num in lat_sscq_10p]

tput_sassy_100p = [885010,1754739
,2313454
,2859355
,3396656
,3649631
,3881710
,3999251
,4132927
,4341999]
lat_sassy_100p  = [8783,9126.4
,9715.2
,10009.6
,10598.4
,10672
,14529228.8
,238169.6
,196144
,394643.2]
tput_sassy_100p = [num / 1E6 for num in tput_sassy_100p]
lat_sassy_100p = [num / 1E3 for num in lat_sassy_100p]

tput_sassy_10p = [887734,1760290
,2597050
,3412643
,4157560
,4885983
,5537433
,5671001
,5744948
,5943389
,6359030
,6676140]
lat_sassy_10p  = [6319,6918.4
,7433.6
,7948.8
,8464
,9052.8
,10524.8
,10598.4
,11040
,12880
,13174.4
,945539.2]
tput_sassy_10p = [num / 1E6 for num in tput_sassy_10p]
lat_sassy_10p = [num / 1E3 for num in lat_sassy_10p]

tput_sassy_JAC1_100p = [881920,1756493
,2325219
,2855986
,3400547
,3634765
,3872249
,4033256]
lat_sassy_JAC1_100p  = [8419,8905.6
,9347.2
,9715.2
,10083.2
,10377.6
,81254.4
,133068.8]
tput_sassy_JAC1_100p = [num / 1E6 for num in tput_sassy_JAC1_100p]
lat_sassy_JAC1_100p = [num / 1E3 for num in lat_sassy_JAC1_100p]

tput_sassy_JAC2_100p = [887053,1758774
,2314530
,2851204
,3304077
,3647534
,3859586
,3991602]
lat_sassy_JAC2_100p  = [8792,9052.8
,9200
,9936
,10292
,10598.4
,10745.6
,249356.8]
tput_sassy_JAC2_100p = [num / 1E6 for num in tput_sassy_JAC2_100p]
lat_sassy_JAC2_100p = [num / 1E3 for num in lat_sassy_JAC2_100p]

tput_sassy_JAC4_100p = [888590,1762331
,2315260
,2871139
,3382181
,3629741
,3747877]
lat_sassy_JAC4_100p  = [8952,9273.6
,9715.2
,10156.8
,10598.4
,10892.8
,35028448]
tput_sassy_JAC4_100p = [num / 1E6 for num in tput_sassy_JAC4_100p]
lat_sassy_JAC4_100p = [num / 1E3 for num in lat_sassy_JAC4_100p]

tput_sassy_JAC1_10p = [882947,1759667
,2602430
,3411695
,4164625
,4863257
,5549525
,5766236
,5954862
,6150319
,6360401
,6542693]
lat_sassy_JAC1_10p  = [6104,6550.4
,6992
,7580.8
,8096
,8758.4
,9641.6
,10009.6
,10377.6
,10892.8
,12217.6
,217120]
tput_sassy_JAC1_10p = [num / 1E6 for num in tput_sassy_JAC1_10p]
lat_sassy_JAC1_10p = [num / 1E3 for num in lat_sassy_JAC1_10p]

tput_sassy_JAC2_10p = [885912,1760001
,2603194
,3399146
,4169148
,4885419
,5552670
,5764636
,5953819
,6321127
,6321127]
lat_sassy_JAC2_10p  = [6490,6844.8
,7360
,7948.8
,8684.8
,9568
,11628.8
,14131.2
,15295.4
,15971.2
,3522864]
tput_sassy_JAC2_10p = [num / 1E6 for num in tput_sassy_JAC2_10p]
lat_sassy_JAC2_10p = [num / 1E3 for num in lat_sassy_JAC2_10p]

tput_sassy_JAC4_10p = [886509,1760458
,2603395
,3401841
,4158384
,4889238
,5559408
,5743309
,5985903
,6306241
,6682269
,6682269]
lat_sassy_JAC4_10p  = [6831,7065.6
,7654.4
,8243.2
,9052.8
,9936
,11040
,11555.2
,12217.6
,13248
,15603.2
,1153164.8]
tput_sassy_JAC4_10p = [num / 1E6 for num in tput_sassy_JAC4_10p]
lat_sassy_JAC4_10p = [num / 1E3 for num in lat_sassy_JAC4_10p]


ax.plot(tput_scq_100p, lat_scq_100p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p, lat_sscq_100p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p, lat_sassy_100p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_100p, lat_sassy_JAC1_100p, '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_100p, lat_sassy_JAC2_100p, '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p, lat_sassy_JAC4_100p, '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')

ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('100%', fontsize=20)

#ax.set_ylabel('p99 latency\n(us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 6)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_rw_LLC_100.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p, lat_scq_10p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p, lat_sscq_10p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p, lat_sassy_10p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_10p, lat_sassy_JAC1_10p, '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_10p, lat_sassy_JAC2_10p, '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p, lat_sassy_JAC4_10p, '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')

ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

ax.set_ylabel('p99 latency\n(us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 7)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_rw_LLC_10.pdf',bbox_inches='tight')

plt.clf()




###REBALANCES

rebal_scq_100p  = [0
,0
,0
,0
,0
,0
,0
,0
,0
,0
,0]

rebal_scq_10p  = [0,0
,0
,0
,0
,0
,0
,0
,0
,0
,0]

rebal_sscq_100p  = [813318, 1601128
,2597077
,3059092
,3279370
,3494952
,3712503
,3900578
,4128352
,4307512
,4509648
,4697197]
rebal_sscq_10p  = [813655,1587985
,2074528
,2522564
,3160233
,3362221
,3569262
,3774097
,3960969
,4160717]

rebal_sassy_100p  = [789604,1578333
,2076567
,2559882
,3030015
,3250212
,2785387
,2457177
,2226101
,1641838]
rebal_sassy_10p  = [714332,1446954
,2069769
,2601085
,3034789
,3401431
,3621478
,3706967
,3737713
,3779716
,3831611
,3765606]

rebal_sassy_JAC1_100p  = [477364,1133598
,1484285
,1846399
,2224139
,2371424
,2168198
,2001950]
rebal_sassy_JAC1_10p  = [359811,943320
,1514098
,1897629
,2255213
,2567947
,2788909
,2852719
,2896746
,2940902
,2971544
,2963105]

rebal_sassy_JAC2_100p  = [103289,379079
,607216
,818923
,1176593
,1118552
,1192152
,703715]
rebal_sassy_JAC2_10p  = [62290,232758
,512065
,802118
,1045036
,1243915
,1398072
,1443893
,1460334
,1503975
,1240310]

rebal_sassy_JAC4_100p  = [3983,28404
,72108
,144812
,229537
,275681
,898439]
rebal_sassy_JAC4_10p  = [1543,12662
,41054
,98499
,196396
,306671
,420630
,458161
,504556
,557704
,623278
,574840]

print(str(len(tput_scq_100p))+" "+str(len(rebal_scq_100p)))
rebal_scq_100p = [num / 1E6 for num in rebal_scq_100p]
rebal_scq_100p = [req / reb if reb != 0 else float('inf') for req, reb in zip(tput_scq_100p, rebal_scq_100p)]
#print(rebal_scq_100p)

print(str(len(tput_sscq_100p))+" "+str(len(rebal_sscq_100p)))
rebal_sscq_100p = [num / 1E6 for num in rebal_sscq_100p]
rebal_sscq_100p  = [req / reb for req, reb in zip(tput_sscq_100p, rebal_sscq_100p)]
#print(rebal_sscq_100p)

print(str(len(tput_sassy_100p))+" "+str(len(rebal_sassy_100p)))
rebal_sassy_100p = [num / 1E6 for num in rebal_sassy_100p]
rebal_sassy_100p  = [req / reb for req, reb in zip(tput_sassy_100p, rebal_sassy_100p)]
#print(rebal_sassy_100p)

print(str(len(tput_sassy_JAC1_100p))+" "+str(len(rebal_sassy_JAC1_100p)))
rebal_sassy_JAC1_100p = [num / 1E6 for num in rebal_sassy_JAC1_100p]
rebal_sassy_JAC1_100p  = [req / reb for req, reb in zip(tput_sassy_JAC1_100p, rebal_sassy_JAC1_100p)]
#print(rebal_sassy_JAC1_100p)

print(str(len(tput_sassy_JAC2_100p))+" "+str(len(rebal_sassy_JAC2_100p)))
rebal_sassy_JAC2_100p = [num / 1E6 for num in rebal_sassy_JAC2_100p]
rebal_sassy_JAC2_100p  = [req / reb for req, reb in zip(tput_sassy_JAC2_100p, rebal_sassy_JAC2_100p)]
#print(rebal_sassy_JAC2_100p)

print(str(len(tput_sassy_JAC4_100p))+" "+str(len(rebal_sassy_JAC4_100p)))
rebal_sassy_JAC4_100p = [num / 1E6 for num in rebal_sassy_JAC4_100p]
rebal_sassy_JAC4_100p  = [req / reb for req, reb in zip(tput_sassy_JAC4_100p, rebal_sassy_JAC4_100p)]
#print(rebal_sassy_JAC4_100p)

print(str(len(tput_scq_10p))+" "+str(len(rebal_scq_10p)))
rebal_scq_10p = [num / 1E6 for num in rebal_scq_10p]
rebal_scq_10p = [req / reb if reb != 0 else float('inf') for req, reb in zip(tput_scq_10p, rebal_scq_10p)]
#print(rebal_scq_10p)

print(str(len(tput_sscq_10p))+" "+str(len(rebal_sscq_10p)))
rebal_sscq_10p = [num / 1E6 for num in rebal_sscq_10p]
rebal_sscq_10p  = [req / reb for req, reb in zip(tput_sscq_10p, rebal_sscq_10p)]
#print(rebal_sscq_10p)

print(str(len(tput_sassy_10p))+" "+str(len(rebal_sassy_10p)))
rebal_sassy_10p = [num / 1E6 for num in rebal_sassy_10p]
rebal_sassy_10p  = [req / reb for req, reb in zip(tput_sassy_10p, rebal_sassy_10p)]
print(rebal_sassy_10p)

print(str(len(tput_sassy_JAC1_10p))+" "+str(len(rebal_sassy_JAC1_10p)))
rebal_sassy_JAC1_10p = [num / 1E6 for num in rebal_sassy_JAC1_10p]
rebal_sassy_JAC1_10p  = [req / reb for req, reb in zip(tput_sassy_JAC1_10p, rebal_sassy_JAC1_10p)]
#print(rebal_sassy_JAC1_10p)

print(str(len(tput_sassy_JAC2_10p))+" "+str(len(rebal_sassy_JAC2_10p)))
rebal_sassy_JAC2_10p = [num / 1E6 for num in rebal_sassy_JAC2_10p]
rebal_sassy_JAC2_10p  = [req / reb for req, reb in zip(tput_sassy_JAC2_10p, rebal_sassy_JAC2_10p)]
#print(rebal_sassy_JAC2_10p)

print(str(len(tput_sassy_JAC4_10p))+" "+str(len(rebal_sassy_JAC4_10p)))
rebal_sassy_JAC4_10p = [num / 1E6 for num in rebal_sassy_JAC4_10p]
rebal_sassy_JAC4_10p  = [req / reb for req, reb in zip(tput_sassy_JAC4_10p, rebal_sassy_JAC4_10p)]
#print(rebal_sassy_JAC4_10p)

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p[:-3], rebal_scq_10p[:-3], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p[:-3], rebal_sscq_10p[:-3], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-3], rebal_sassy_10p[:-3], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_10p[:-3], rebal_sassy_JAC1_10p[:-3], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_10p[:-3], rebal_sassy_JAC2_10p[:-3], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-3], rebal_sassy_JAC4_10p[:-3], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
ax.set_xlim(0, 7)

#ax.set_ylabel('Requests/rebalanes',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_rw_LLC_rebal_10.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p, rebal_scq_100p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p[:-2], rebal_sscq_100p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-2], rebal_sassy_100p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_100p[:-2], rebal_sassy_JAC1_100p[:-2], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_100p[:-1], rebal_sassy_JAC2_100p[:-1], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-1], rebal_sassy_JAC4_100p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
ax.set_xlim(0, 6)

#ax.set_ylabel('Requests/\nrebalance',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_rw_LLC_rebal_100.pdf',bbox_inches='tight')

plt.clf()

ast_scq_100p = [841
,827
,827
,824
,850
,821
,822
,852
,842
,867
,867]
ast_scq_100p = [num / 1E3 for num in ast_scq_100p]

ast_sscq_100p = [1040,1042
,1053
,1049
,1052
,1048
,1048
,1049
,1047
,1045
,1043
,1042]
ast_sscq_100p = [num / 1E3 for num in ast_sscq_100p]

ast_sassy_JSC_100p = [1005,1002
,1007
,1006
,1008
,1024
,1011
,992
,994
,971]
ast_sassy_JSC_100p = [num / 1E3 for num in ast_sassy_JSC_100p]

ast_sassy_JAC1_100p  = [977,981
,984
,991
,992
,992
,998
,997]
ast_sassy_JAC1_100p = [num / 1E3 for num in ast_sassy_JAC1_100p]

ast_sassy_JAC2_100p  = [932,937
,934
,949
,945
,959
,962
,938]
ast_sassy_JAC2_100p = [num / 1E3 for num in ast_sassy_JAC2_100p]

ast_sassy_JAC4_100p  = [885,887
,895
,904
,903
,910
,949]
ast_sassy_JAC4_100p = [num / 1E3 for num in ast_sassy_JAC4_100p]

ast_scq_10p = [775,779
,769
,758
,755
,749
,753
,751
,745
,758
,775]
ast_scq_10p = [num / 1E3 for num in ast_scq_10p]

ast_sscq_10p = [962,966
,977
,982
,993
,985
,989
,997
,997
,990]
ast_sscq_10p = [num / 1E3 for num in ast_sscq_10p]

ast_sassy_JSC_10p = [953,956
,905
,909
,918
,929
,967
,959
,946
,947
,949
,954]
ast_sassy_JSC_10p = [num / 1E3 for num in ast_sassy_JSC_10p]

ast_sassy_JAC1_10p  = [890,892
,896
,898
,903
,921
,932
,938
,938
,943
,946
,947]
ast_sassy_JAC1_10p = [num / 1E3 for num in ast_sassy_JAC1_10p]

ast_sassy_JAC2_10p  = [873,876
,886
,890
,918
,928
,936
,935
,941
,947
,904]
ast_sassy_JAC2_10p = [num / 1E3 for num in ast_sassy_JAC2_10p]

ast_sassy_JAC4_10p  = [790,799
,809
,818
,835
,856
,886
,890
,907
,912
,924
,910]
ast_sassy_JAC4_10p = [num / 1E3 for num in ast_sassy_JAC4_10p]


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p[:-2], ast_scq_100p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p, ast_sscq_100p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-1], ast_sassy_JSC_100p[:-1], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_100p[:-1], ast_sassy_JAC1_100p[:-1], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_100p[:-1], ast_sassy_JAC2_100p[:-1], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-1], ast_sassy_JAC4_100p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

#ax.set_ylabel('Avg Service\nTime (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 2.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)

ax.set_xlim(0, 6)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)

#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_rw_LLC_ast_100.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p[:-3], ast_scq_10p[:-3], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p[:-3], ast_sscq_10p[:-3], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-4], ast_sassy_JSC_10p[:-4], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_10p[:-4], ast_sassy_JAC1_10p[:-4], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_10p[:-4], ast_sassy_JAC2_10p[:-4], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-4], ast_sassy_JAC4_10p[:-4], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
ax.set_xlim(0, 7)

ax.set_ylabel('Avg Service\nTime (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 1.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)
#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_rw_LLC_ast_10.pdf',bbox_inches='tight')