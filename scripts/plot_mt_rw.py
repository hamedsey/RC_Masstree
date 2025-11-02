import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

tput_scq_100p = [881239,1754624
,2320511
,2859847
,3112478
,3373961
,3638408
,3871762
,4115075
,4392840
,4552218
,4773599
,5009038
,5267848]
lat_scq_100p  = [8514,8832
,9420.8
,9936
,10083.2
,10598.4
,11113.6
,11408
,12070.4
,12806.4
,13248
,15308.8
,51078.4
,157798.4]
tput_scq_100p = [num / 1E6 for num in tput_scq_100p]
lat_scq_100p = [num / 1E3 for num in lat_scq_100p]

tput_scq_10p = [883475,1759137
,3404091
,3656740
,3916984
,4158932
,5076442
,5271042
,5473497
,6748107]
lat_scq_10p  = [6521,6771.2
,11628.8
,14572.8
,18915.2
,329286.4
,1443590.4
,1474060.8
,1443222.4
,1380809.6]
tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
lat_scq_10p = [num / 1E3 for num in lat_scq_10p]
    
tput_sscq_100p = [884920,1756503
,2314775
,2866898
,3385495
,3637200
,3881531
,4115919
,4306599
,4571423
,4754823
,4945346
,5217887]
lat_sscq_100p  = [11860,13984
,21491.2
,23993.6
,24582.4
,24876.8
,25171.2
,25760
,26128
,27305.6
,29881.6
,32310.4
,112755.2]
tput_sscq_100p = [num / 1E6 for num in tput_sscq_100p]
lat_sscq_100p = [num / 1E3 for num in lat_sscq_100p]

tput_sscq_10p = [889321,1759791
,2323986
,2867864
,3665440
,3899408
,4149726
,4401022
,4617704
,4804558
,5028400
,5205440]
lat_sscq_10p  = [9957,10672
,15235.2
,18694.4
,29366.4
,35033.6
,86700.8
,1623689.6
,1430048
,1641206.4
,1626412.8
,1559952]
tput_sscq_10p = [num / 1E6 for num in tput_sscq_10p]
lat_sscq_10p = [num / 1E3 for num in lat_sscq_10p]

tput_sassy_100p = [885684,1756121
,2319770
,2867519
,3366515
,3644697
,3869046
,4000083
,4076886
,4335275]
lat_sassy_100p  = [9478,9641.6
,10009.6
,10598.4
,10966.4
,11408
,11481.6
,86480
,27188502.4
,326857.6]
tput_sassy_100p = [num / 1E6 for num in tput_sassy_100p]
lat_sassy_100p = [num / 1E3 for num in lat_sassy_100p]

tput_sassy_10p = [888809,1760135
,2597594
,3414742
,4160827
,4859205
,5521969
,5748925
,5989013
,6325050
,6675827]
lat_sassy_10p  = [6849,7139.2
,7875.2
,8390.4
,9052.8
,9936
,11408
,12364.8
,14278.4
,17001.6
,833446.4]
tput_sassy_10p = [num / 1E6 for num in tput_sassy_10p]
lat_sassy_10p = [num / 1E3 for num in lat_sassy_10p]

tput_sassy_JAC1_100p = [885663,1755752
,2312647
,2862157
,3395933
,3654218
,3890191
,4025193
,3985094
,4329995
,4571457
,4771382
,5040450
,5097456]
lat_sassy_JAC1_100p  = [8809,9273.6
,9788.8
,10230.4
,10745.6
,11113.6
,11334.4
,304336
,36893692.8
,13444880
,599766.4
,12683708.8
,1851849.6
,18215852.8]
tput_sassy_JAC1_100p = [num / 1E6 for num in tput_sassy_JAC1_100p]
lat_sassy_JAC1_100p = [num / 1E3 for num in lat_sassy_JAC1_100p]

tput_sassy_JAC2_100p = [880312,1756428
,2314298
,2853193
,3399111
,3639800
,3888386
,4012908
,4113537]
lat_sassy_JAC2_100p  = [8904,9200
,9715.2
,10377.6
,10892.8
,11334.4
,11481.6
,382352
,553692.8]
tput_sassy_JAC2_100p = [num / 1E6 for num in tput_sassy_JAC2_100p]
lat_sassy_JAC2_100p = [num / 1E3 for num in lat_sassy_JAC2_100p]

tput_sassy_JAC4_100p = [886386,1756262
,2317046
,2857075
,3403618
,3650705
,3867157
,4111563]
lat_sassy_JAC4_100p  = [9740,9936
,10451.2
,11040
,11776
,12144
,255833.6
,354899.2]
tput_sassy_JAC4_100p = [num / 1E6 for num in tput_sassy_JAC4_100p]
lat_sassy_JAC4_100p = [num / 1E3 for num in lat_sassy_JAC4_100p]

tput_sassy_JAC1_10p = [886795,1759191
,2595809
,3410876
,4167772
,4857771
,5562701
,5757089
,6159618
,6323282
,6507395
,6677293]
lat_sassy_JAC1_10p  = [6652,6918.4
,7433.6
,8096
,8758.4
,9568
,11040
,11702.4
,13836.8
,21932.8
,23036.8
,359683.2]
tput_sassy_JAC1_10p = [num / 1E6 for num in tput_sassy_JAC1_10p]
lat_sassy_JAC1_10p = [num / 1E3 for num in lat_sassy_JAC1_10p]

tput_sassy_JAC2_10p = [885960,1762167
,2597667
,3399533
,4165942
,4855988
,5528000
,5736433
,5971501
,6139151
,6495581
,5734198]
lat_sassy_JAC2_10p  = [6904,7065.6
,7728
,8537.6
,9494.4
,10676.8
,11477.6
,12217.6
,13984
,14425.6
,19062.4
,3287638.4]
tput_sassy_JAC2_10p = [num / 1E6 for num in tput_sassy_JAC2_10p]
lat_sassy_JAC2_10p = [num / 1E3 for num in lat_sassy_JAC2_10p]

tput_sassy_JAC4_10p = [887956,1760276
,2604280
,3402058
,4161272
,4878465
,5564266
,5732231
,5952421
,6119675
,6315542
,6479104
,4254768]
lat_sassy_JAC4_10p  = [7455,7580.8
,8243.2
,8979.2
,9936
,11260.8
,13100.8
,14958.4
,15603.2
,16184.8
,17811.2
,18915.2
,4303612.8]
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

plt.savefig('mt_rw_100.pdf',bbox_inches='tight')

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

plt.savefig('mt_rw_10.pdf',bbox_inches='tight')

plt.clf()




###REBALANCES

rebal_scq_100p  = [0,0
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

rebal_scq_10p  = [0,0
,0
,0
,0
,0
,0
,0
,0
,0]

rebal_sscq_100p  = [819887,1601720
,2105967
,2599777
,3067220
,3294065
,3513921
,3727162
,3897448
,4139762
,4302419
,4474753
,4713096]
rebal_sscq_10p  = [815509,1585042
,2070774
,2507357
,3158344
,3350328
,3558909
,3771288
,3955806
,4112684
,4304776
,4457571]

rebal_sassy_100p  = [784561,1579407
,2081849
,2565253
,3002026
,3242588
,3436283
,3160760
,2434027
,1651231]
rebal_sassy_10p  = [712300,1445501
,2062072
,2588095
,3003999
,3330094
,3557150
,3601426
,3606513
,3554267
,3171371]

rebal_sassy_JAC1_100p  = [470099,1131065
,1470465
,1847729
,2205876
,2361992
,2506013
,2094631
,1736552
,1699531
,1327064
,1112343
,1553916
,1964974]
rebal_sassy_JAC1_10p  = [353221,955413
,1503669
,1878026
,2238423
,2520288
,2718991
,2760105
,2808738
,2762177
,2668537
,2587291]

rebal_sassy_JAC2_100p  = [102775,383095
,612500
,818416
,1015597
,1104173
,1182243
,1055680
,1088576]
rebal_sassy_JAC2_10p  = [68312,241217
,520586
,812470
,1043099
,1210217
,1359738
,1416375
,1453398
,1476857
,1490042
,1305533]

rebal_sassy_JAC4_100p  = [3096,30674
,79740
,155533
,250225
,296004
,422742
,529797]
rebal_sassy_JAC4_10p  = [1294,12824
,45116
,112855
,214356
,338856
,461937
,487028
,529649
,555117
,595143
,623095
,645885]

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

plt.savefig('mt_rw_rebal_10.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p[:-2], rebal_scq_100p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p[:-2], rebal_sscq_100p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-2], rebal_sassy_100p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_100p[:-5], rebal_sassy_JAC1_100p[:-5], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_100p[:-2], rebal_sassy_JAC2_100p[:-2], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-1], rebal_sassy_JAC4_100p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
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

plt.savefig('mt_rw_rebal_100.pdf',bbox_inches='tight')

plt.clf()

ast_scq_100p = [1045,1047
,1052
,1059
,1062
,1060
,1061
,1060
,1063
,1065
,1064
,1064
,1068
,1067]
ast_scq_100p = [num / 1E3 for num in ast_scq_100p]

ast_sscq_100p = [1205,1210
,1206
,1206
,1189
,1183
,1171
,1166
,1163
,1156
,1150
,1146
,1144]
ast_sscq_100p = [num / 1E3 for num in ast_sscq_100p]

ast_sassy_JSC_100p = [1157,1159
,1157
,1153
,1148
,1146
,1142
,1139
,1138
,1121]
ast_sassy_JSC_100p = [num / 1E3 for num in ast_sassy_JSC_100p]

ast_sassy_JAC1_100p  = [1145,1149
,1151
,1146
,1140
,1146
,1144
,1139
,1129
,1129
,1120
,1116
,1118
,1119]
ast_sassy_JAC1_100p = [num / 1E3 for num in ast_sassy_JAC1_100p]

ast_sassy_JAC2_100p  = [1142,1145
,1124
,1123
,1126
,1128
,1125
,1122
,1125]
ast_sassy_JAC2_100p = [num / 1E3 for num in ast_sassy_JAC2_100p]

ast_sassy_JAC4_100p  = [1113,1115
,1106
,1108
,1112
,1107
,1113
,1121]
ast_sassy_JAC4_100p = [num / 1E3 for num in ast_sassy_JAC4_100p]

ast_scq_10p = [910,913
,920
,935
,945
,958
,964
,974
,976
,984]
ast_scq_10p = [num / 1E3 for num in ast_scq_10p]

ast_sscq_10p = [1096,1101
,1099
,1101
,1087
,1082
,1081
,1075
,1077
,1078
,1077
,1080]
ast_sscq_10p = [num / 1E3 for num in ast_sscq_10p]

ast_sassy_JSC_10p = [1047,1057
,1035
,1040
,1040
,1039
,1039
,1038
,1040
,1040
,1036]
ast_sassy_JSC_10p = [num / 1E3 for num in ast_sassy_JSC_10p]

ast_sassy_JAC1_10p  = [1018,1021
,1030
,1033
,1038
,1038
,1042
,1038
,1038
,1039
,1038
,1035]
ast_sassy_JAC1_10p = [num / 1E3 for num in ast_sassy_JAC1_10p]

ast_sassy_JAC2_10p  = [1015,1017
,1035
,1046
,1040
,1044
,1041
,1032
,1032
,1037
,1040
,1030]
ast_sassy_JAC2_10p = [num / 1E3 for num in ast_sassy_JAC2_10p]

ast_sassy_JAC4_10p  = [940,938
,955
,979
,999
,1009
,1016
,1018
,1020
,1019
,1024
,1026
,1028]
ast_sassy_JAC4_10p = [num / 1E3 for num in ast_sassy_JAC4_10p]


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p[:-3], ast_scq_100p[:-3], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p[:-2], ast_sscq_100p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-2], ast_sassy_JSC_100p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_100p[:-6], ast_sassy_JAC1_100p[:-6], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
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

plt.savefig('mt_rw_ast_100.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p[:-2], ast_scq_10p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p[:-2], ast_sscq_10p[:-2], '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-3], ast_sassy_JSC_10p[:-3], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
#ax.plot(tput_sassy_JAC1_10p[:-3], ast_sassy_JAC1_10p[:-3], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
#ax.plot(tput_sassy_JAC2_10p[:-3], ast_sassy_JAC2_10p[:-3], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-3], ast_sassy_JAC4_10p[:-3], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
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

plt.savefig('mt_rw_ast_10.pdf',bbox_inches='tight')