import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

tput_scq_100p = [888649,1758191
,2591261
,3390512
,4136747
,4888290
,5081225
,5250077
,5403433]

lat_scq_100p  = [8109,8316.8
,9052.8
,9862.4
,10672
,12291.2
,17148.8
,57113.6
,167292.8]
tput_scq_100p = [num / 1E6 for num in tput_scq_100p]
lat_scq_100p = [num / 1E3 for num in lat_scq_100p]

tput_scq_10p = [887094,1760944
,2326059
,2874900
,3149126
,3410677
,3680258
,3933341
,4182432
,4415678
,4653714
,4893466
,5099856
,5307942]
lat_scq_10p  = [6107,6256
,6697.6
,7433.6
,8096
,8390.4
,9788.8
,10156
,15824
,1427766.4
,1267465.6
,1199238.4
,1483260.8
,1481273.6]
tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
lat_scq_10p = [num / 1E3 for num in lat_scq_10p]
    
tput_sscq_100p = [886794,1757515
,2856291
,3374244
,3651869
,3860486
,4113347
,4344862
,4615204
,4819966
,5065199
,5270677]
lat_sscq_100p  = [12091,13248
,22448
,23110.4
,23331.2
,23404.8
,23920
,23920
,25024
,26201.6
,40553.6
,99948.8]
tput_sscq_100p = [num / 1E6 for num in tput_sscq_100p]
lat_sscq_100p = [num / 1E3 for num in lat_sscq_100p]

tput_sscq_10p = [884432,1761765
,2331240
,2880033
,3420597
,3346776
,3933769
,4162146
,4418634
,4628485
,4858814
,5091091
,5282463]
lat_sscq_10p  = [9905,10304
,13689.6
,16560
,18473.6
,18620.8
,24435.2
,48060.8
,1513878.4
,1415328
,1509462.4
,1438585.6
,1552076.8]
tput_sscq_10p = [num / 1E6 for num in tput_sscq_10p]
lat_sscq_10p = [num / 1E3 for num in lat_sscq_10p]

tput_sassy_100p = [884450,1762468
,2322971
,2858833
,3388043
,3635321
,3873962
,3990278
,4092146
,4332511]
lat_sassy_100p  = [9004,9200
,9641.6
,10156.8
,10598.4
,10966.4
,15235.2
,5968003.2
,34955216
,2235011.2]
tput_sassy_100p = [num / 1E6 for num in tput_sassy_100p]
lat_sassy_100p = [num / 1E3 for num in lat_sassy_100p]

tput_sassy_10p = [888790,1760442
,2600007
,3410088
,4173332
,4908840
,5614467
,5814845
,6228894
,6365880
,6573460
,6787382]
lat_sassy_10p  = [6594,6844.8
,7360
,7948.8
,8464
,9200
,10230.4
,10892.8
,12144
,12512
,23846.4
,50540.8]
tput_sassy_10p = [num / 1E6 for num in tput_sassy_10p]
lat_sassy_10p = [num / 1E3 for num in lat_sassy_10p]

tput_sassy_JAC1_100p = [881293,1757669
,2317732
,2857487
,3403656
,3885341
,4014275
,3903789
,4368871
,4594593]
lat_sassy_JAC1_100p  = [9108,9200
,9715.2
,10156.8
,10745.6
,11040
,191507.2
,39746796.8
,203283.2
,469936]
tput_sassy_JAC1_100p = [num / 1E6 for num in tput_sassy_JAC1_100p]
lat_sassy_JAC1_100p = [num / 1E3 for num in lat_sassy_JAC1_100p]

tput_sassy_JAC2_100p = [889032,1763279
,2319777
,2860200
,3401683
,3661060
,3804199
,4132952
,4329618]
lat_sassy_JAC2_100p  = [8792,8905.6
,9420.8
,9936
,10524.8
,45264
,29009734.4
,187385.6
,26532064]
tput_sassy_JAC2_100p = [num / 1E6 for num in tput_sassy_JAC2_100p]
lat_sassy_JAC2_100p = [num / 1E3 for num in lat_sassy_JAC2_100p]

tput_sassy_JAC4_100p = [884512,1758071
,2328429
,2860791
,3396542
,3586966
,3645490
,4133328
,4788858
,5040892]
lat_sassy_JAC4_100p  = [9221,9494.4
,9936
,10451.2
,11113.6
,11481.6
,27491072
,522486.4
,2705315.2
,9369059.2]
tput_sassy_JAC4_100p = [num / 1E6 for num in tput_sassy_JAC4_100p]
lat_sassy_JAC4_100p = [num / 1E3 for num in lat_sassy_JAC4_100p]

tput_sassy_JAC1_10p = [876941,1760867
,2604624
,3408235
,4164229
,4901581
,5599857
,6019407
,6214466
,6529094
,6255337]
lat_sassy_JAC1_10p  = [6609,6844.8
,7433.6
,8022.4
,8684.8
,9494.4
,11260.8
,13321.6
,15308.8
,1211161.6
,3122038.4]
tput_sassy_JAC1_10p = [num / 1E6 for num in tput_sassy_JAC1_10p]
lat_sassy_JAC1_10p = [num / 1E3 for num in lat_sassy_JAC1_10p]

tput_sassy_JAC2_10p = [883412,1761094
,2605145
,3408488
,4177813
,4890389
,5571116
,5808001
,6227278
,6361637
,6552758
,6588417]
lat_sassy_JAC2_10p  = [6144,6476.8
,6992
,7580.8
,8169.6
,8905.6
,9862.4
,10230.4
,11260.8
,11849.6
,12291.2
,3990886.4]
tput_sassy_JAC2_10p = [num / 1E6 for num in tput_sassy_JAC2_10p]
lat_sassy_JAC2_10p = [num / 1E3 for num in lat_sassy_JAC2_10p]

tput_sassy_JAC4_10p = [889345,1761373
,2609226
,3406843
,4185320
,4890523
,5571129
,5779628
,5999647
,6221276
,6410460
,6585156
,6735214
,6755214]
lat_sassy_JAC4_10p  = [6991,7139.2
,7728
,8316.8
,9052.8
,10009.6
,11334.4
,11702.4
,12217.6
,13542.4
,13542.4
,14278.4
,15456
,105455.99]
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

ax.set_ylabel('p99 latency\n(us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 6)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_ro_100.pdf',bbox_inches='tight')

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

#ax.set_ylabel('p99 latency (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 7)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_ro_10.pdf',bbox_inches='tight')

plt.clf()




###REBALANCES

rebal_scq_100p  = [0,0
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
,0
,0
,0
,0]

rebal_sscq_100p  = [817450,1602834
,2591439
,3058033
,3308061
,3496567
,3719657
,3933898
,4178329
,4362745
,4575126
,4756455]
rebal_sscq_10p  = [819966,1586518
,2080079
,2531000
,2962958
,3098408
,3383552
,3573256
,3787143
,3967295
,4156683
,4347927
,4522961]

rebal_sassy_100p  = [788509,1585170
,2085401
,2559036
,3022388
,3236662
,3427183
,1929735
,1864559
,1735127]
rebal_sassy_10p  = [714668,1447149
,2072196
,2597586
,3039532
,3400365
,3666326
,3718167
,3794321
,3810742
,3747997
,3665406]

rebal_sassy_JAC1_100p  = [476345,1131695
,1474423
,1842108
,2212571
,2508020
,1778929
,1856339
,1542699
,1309775]
rebal_sassy_JAC1_10p  = [356097,949381
,1510292
,1883864
,2242089
,2554757
,2762449
,2853786
,2880076
,2816790
,2195001]

rebal_sassy_JAC2_100p  = [103559,377404
,610070
,815643
,1016939
,1031231
,1148415
,1107898
,1032680]
rebal_sassy_JAC2_10p  = [64553,231642
,506866
,802688
,1042178
,1247089
,1415645
,1459432
,1535887
,1551379
,1583511
,1304373]

rebal_sassy_JAC4_100p  = [3759,28068
,74807
,146496
,238701
,285005
,470019
,558496
,911829
,786583]
rebal_sassy_JAC4_10p  = [1992,11566
,42292
,100894
,202940
,310030
,432294
,468674
,508242
,545994
,586559
,613063
,637347
,647347]

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
#print(rebal_sassy_10p)

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
ax.plot(tput_scq_10p[:-2], rebal_scq_10p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p[:-2], rebal_sscq_10p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-2], rebal_sassy_10p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_10p[:-2], rebal_sassy_JAC1_10p[:-2], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_10p[:-2], rebal_sassy_JAC2_10p[:-2], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-2], rebal_sassy_JAC4_10p[:-2], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
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

plt.savefig('mt_ro_rebal_10.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p[:-2], rebal_scq_100p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p[:-2], rebal_sscq_100p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-3], rebal_sassy_100p[:-3], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_100p[:-4], rebal_sassy_JAC1_100p[:-4], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_100p[:-2], rebal_sassy_JAC2_100p[:-2], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-2], rebal_sassy_JAC4_100p[:-2], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
ax.set_xlim(0, 6)

ax.set_ylabel('Requests/\nrebalance',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_ro_rebal_100.pdf',bbox_inches='tight')

plt.clf()

ast_scq_100p = [932,934
,956
,982
,990
,1002
,1006
,1008
,1007]
ast_scq_100p = [num / 1E3 for num in ast_scq_100p]

ast_sscq_100p = [1102,1106
,1098
,1093
,1088
,1085
,1081
,1077
,1076
,1072
,1074
,0]
ast_sscq_100p = [num / 1E3 for num in ast_sscq_100p]

ast_sassy_JSC_100p = [1052,1059
,1061
,1066
,1066
,1069
,1062
,1059
,1054
,1049]
ast_sassy_JSC_100p = [num / 1E3 for num in ast_sassy_JSC_100p]

ast_sassy_JAC1_100p  = [1068,1070
,1076
,1083
,1077
,1067
,1048
,1046
,1052
,1050]
ast_sassy_JAC1_100p = [num / 1E3 for num in ast_sassy_JAC1_100p]

ast_sassy_JAC2_100p  = [1010,1013
,1028
,1034
,1049
,1039
,1039
,1042
,1048]
ast_sassy_JAC2_100p = [num / 1E3 for num in ast_sassy_JAC2_100p]

ast_sassy_JAC4_100p  = [988,991
,1003
,1014
,1023
,1030
,1038
,1034
,1042
,1039]
ast_sassy_JAC4_100p = [num / 1E3 for num in ast_sassy_JAC4_100p]

ast_scq_10p = [808,811
,807
,802
,807
,804
,804
,809
,812
,822
,829
,843
,852
,864]
ast_scq_10p = [num / 1E3 for num in ast_scq_10p]

ast_sscq_10p = [980,985
,997
,1006
,1014
,1012
,1010
,1013
,1011
,1007
,1013
,1013
,1020]
ast_sscq_10p = [num / 1E3 for num in ast_sscq_10p]

ast_sassy_JSC_10p = [923,920
,928
,953
,955
,964
,973
,975
,979
,977
,979
,978]
ast_sassy_JSC_10p = [num / 1E3 for num in ast_sassy_JSC_10p]

ast_sassy_JAC1_10p  = [930,932
,944
,963
,975
,991
,977
,981
,985
,984
,983]
ast_sassy_JAC1_10p = [num / 1E3 for num in ast_sassy_JAC1_10p]

ast_sassy_JAC2_10p  = [863,865
,890
,910
,925
,942
,952
,953
,960
,961
,965
,942]
ast_sassy_JAC2_10p = [num / 1E3 for num in ast_sassy_JAC2_10p]

ast_sassy_JAC4_10p  = [830,831
,837
,851
,880
,908
,932
,929
,934
,944
,945
,947
,949
,951]
ast_sassy_JAC4_10p = [num / 1E3 for num in ast_sassy_JAC4_10p]


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p[:-2], ast_scq_100p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p[:-2], ast_sscq_100p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-2], ast_sassy_JSC_100p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_100p[:-2], ast_sassy_JAC1_100p[:-2], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_100p[:-2], ast_sassy_JAC2_100p[:-2], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-2], ast_sassy_JAC4_100p[:-2], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

ax.set_ylabel('Avg Service\nTime (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 2.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)

ax.set_xlim(0, 6)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)

#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_ro_ast_100.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p[:-2], ast_scq_10p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p[:-2], ast_sscq_10p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-2], ast_sassy_JSC_10p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_10p[:-2], ast_sassy_JAC1_10p[:-2], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_10p[:-2], ast_sassy_JAC2_10p[:-2], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-2], ast_sassy_JAC4_10p[:-2], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
ax.set_xlim(0, 7)

#ax.set_ylabel('Avg Service Time',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
#ax.set_ylim(0, 1.5)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)
#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_ro_ast_10.pdf',bbox_inches='tight')