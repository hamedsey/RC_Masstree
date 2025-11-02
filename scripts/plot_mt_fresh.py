import matplotlib.pyplot as plt
import os
import numpy as np
#if not  os.path.exists('graphs_bm_r5_p10_30x_SLO'):
#	os.makedirs('graphs_bm_r5_p10_30x_SLO')
from matplotlib.ticker import FuncFormatter

#load imbalance
figure, ((ax)) = plt.subplots(1, 1)

tput_scq_100p = []
lat_scq_100p  = []
tput_scq_100p = [num / 1E6 for num in tput_scq_100p]
lat_scq_100p = [num / 1E3 for num in lat_scq_100p]

tput_scq_10p = []
lat_scq_10p  = []
tput_scq_10p = [num / 1E6 for num in tput_scq_10p]
lat_scq_10p = [num / 1E3 for num in lat_scq_10p]
    
tput_sscq_100p = []
lat_sscq_100p  = []
tput_sscq_100p = [num / 1E6 for num in tput_sscq_100p]
lat_sscq_100p = [num / 1E3 for num in lat_sscq_100p]

tput_sscq_10p = []
lat_sscq_10p  = []
tput_sscq_10p = [num / 1E6 for num in tput_sscq_10p]
lat_sscq_10p = [num / 1E3 for num in lat_sscq_10p]

tput_sassy_100p = []
lat_sassy_100p  = []
tput_sassy_100p = [num / 1E6 for num in tput_sassy_100p]
lat_sassy_100p = [num / 1E3 for num in lat_sassy_100p]

tput_sassy_10p = []
lat_sassy_10p  = []
tput_sassy_10p = [num / 1E6 for num in tput_sassy_10p]
lat_sassy_10p = [num / 1E3 for num in lat_sassy_10p]

tput_sassy_JAC1_100p = []
lat_sassy_JAC1_100p  = []
tput_sassy_JAC1_100p = [num / 1E6 for num in tput_sassy_JAC1_100p]
lat_sassy_JAC1_100p = [num / 1E3 for num in lat_sassy_JAC1_100p]

tput_sassy_JAC2_100p = []
lat_sassy_JAC2_100p  = []
tput_sassy_JAC2_100p = [num / 1E6 for num in tput_sassy_JAC2_100p]
lat_sassy_JAC2_100p = [num / 1E3 for num in lat_sassy_JAC2_100p]

tput_sassy_JAC4_100p = []
lat_sassy_JAC4_100p  = []
tput_sassy_JAC4_100p = [num / 1E6 for num in tput_sassy_JAC4_100p]
lat_sassy_JAC4_100p = [num / 1E3 for num in lat_sassy_JAC4_100p]

tput_sassy_JAC1_10p = []
lat_sassy_JAC1_10p  = []
tput_sassy_JAC1_10p = [num / 1E6 for num in tput_sassy_JAC1_10p]
lat_sassy_JAC1_10p = [num / 1E3 for num in lat_sassy_JAC1_10p]

tput_sassy_JAC2_10p = []
lat_sassy_JAC2_10p  = []
tput_sassy_JAC2_10p = [num / 1E6 for num in tput_sassy_JAC2_10p]
lat_sassy_JAC2_10p = [num / 1E3 for num in lat_sassy_JAC2_10p]

tput_sassy_JAC4_10p = []
lat_sassy_JAC4_10p  = []
tput_sassy_JAC4_10p = [num / 1E6 for num in tput_sassy_JAC4_10p]
lat_sassy_JAC4_10p = [num / 1E3 for num in lat_sassy_JAC4_10p]


ax.plot(tput_scq_100p, lat_scq_100p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p, lat_sscq_100p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p, lat_sassy_100p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_100p, lat_sassy_JAC1_100p, '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_100p, lat_sassy_JAC2_100p, '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p, lat_sassy_JAC4_100p, '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')

ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('100%', fontsize=20)

ax.set_ylabel('p99 latency\n(us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 5)
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
ax.plot(tput_sassy_JAC1_10p, lat_sassy_JAC1_10p, '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_10p, lat_sassy_JAC2_10p, '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p, lat_sassy_JAC4_10p, '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')

ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)

#ax.set_ylabel('p99 latency (us)',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(0, 50)
ax.set_xlim(0, 6)

#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_ro_10.pdf',bbox_inches='tight')

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
,0
,0
,0
,0
,0
,0
,0]

rebal_scq_10p  = [0
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

rebal_sscq_100p  = []
rebal_sscq_10p  = []

rebal_sassy_100p  = []
rebal_sassy_10p  = []

rebal_sassy_JAC1_100p  = []
rebal_sassy_JAC1_10p  = []

rebal_sassy_JAC2_100p  = []
rebal_sassy_JAC2_10p  = []

rebal_sassy_JAC4_100p  = []
rebal_sassy_JAC4_10p  = []

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
ax.plot(tput_scq_10p, rebal_scq_10p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p, rebal_sscq_10p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p, rebal_sassy_10p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_10p, rebal_sassy_JAC1_10p, '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_10p, rebal_sassy_JAC2_10p, '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p, rebal_sassy_JAC4_10p, '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
ax.set_xlim(0, 6)

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
ax.plot(tput_scq_100p, rebal_scq_100p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p, rebal_sscq_100p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p, rebal_sassy_100p, '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_100p[:-1], rebal_sassy_JAC1_100p[:-1], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_100p[:-1], rebal_sassy_JAC2_100p[:-1], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-1], rebal_sassy_JAC4_100p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
plt.yscale('log')
ax.set_xlim(0, 5)

ax.set_ylabel('Requests/\nrebalance',fontsize=20)
ax.set_xlabel('Load (MRPS)',fontsize=20)
ax.set_ylim(1, 1000)
#ax.set_xscale('log', basex=2)
#ax.set_yscale('log', basey=2)


#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_ro_rebal_100.pdf',bbox_inches='tight')

plt.clf()

ast_scq_100p = []
ast_scq_100p = [num / 1E3 for num in ast_scq_100p]

ast_sscq_100p = []
ast_sscq_100p = [num / 1E3 for num in ast_sscq_100p]

ast_sassy_JSC_100p = []
ast_sassy_JSC_100p = [num / 1E3 for num in ast_sassy_JSC_100p]

ast_sassy_JAC1_100p  = []
ast_sassy_JAC1_100p = [num / 1E3 for num in ast_sassy_JAC1_100p]

ast_sassy_JAC2_100p  = []
ast_sassy_JAC2_100p = [num / 1E3 for num in ast_sassy_JAC2_100p]

ast_sassy_JAC4_100p  = []
ast_sassy_JAC4_100p = [num / 1E3 for num in ast_sassy_JAC4_100p]

ast_scq_10p = []
ast_scq_10p = [num / 1E3 for num in ast_scq_10p]

ast_sscq_10p = []
ast_sscq_10p = [num / 1E3 for num in ast_sscq_10p]

ast_sassy_JSC_10p = []
ast_sassy_JSC_10p = [num / 1E3 for num in ast_sassy_JSC_10p]

ast_sassy_JAC1_10p  = []
ast_sassy_JAC1_10p = [num / 1E3 for num in ast_sassy_JAC1_10p]

ast_sassy_JAC2_10p  = []
ast_sassy_JAC2_10p = [num / 1E3 for num in ast_sassy_JAC2_10p]

ast_sassy_JAC4_10p  = []
ast_sassy_JAC4_10p = [num / 1E3 for num in ast_sassy_JAC4_10p]


plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_100p[:-2], ast_scq_100p[:-2], '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_100p, ast_sscq_100p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_100p[:-2], ast_sassy_JSC_100p[:-2], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_100p[:-3], ast_sassy_JAC1_100p[:-3], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_100p[:-3], ast_sassy_JAC2_100p[:-3], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_100p[:-3], ast_sassy_JAC4_100p[:-3], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
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

ax.set_xlim(0, 5)
formatter = FuncFormatter(lambda y, _: f'{y:.1f}')
ax.yaxis.set_major_formatter(formatter)

#figure.tight_layout(pad=1.0)
figure.set_size_inches(3.5, 2.0)

plt.savefig('mt_ro_ast_100.pdf',bbox_inches='tight')

plt.clf()

figure, ((ax)) = plt.subplots(1, 1)
ax.plot(tput_scq_10p, ast_scq_10p, '--', color ='orchid', linewidth = 2.0, label="grp_ex", marker='o')
ax.plot(tput_sscq_10p, ast_sscq_10p, '--', color= 'purple', linewidth = 2.0, label="grp_sh", marker='o')
ax.plot(tput_sassy_10p[:-1], ast_sassy_JSC_10p[:-1], '-', color= 'navy', linewidth = 2.0, label="Sassy_JSC", marker='o')
ax.plot(tput_sassy_JAC1_10p[:-1], ast_sassy_JAC1_10p[:-1], '-', color= 'blue', linewidth = 2.0, label="Sassy_JAC", marker='o')
ax.plot(tput_sassy_JAC2_10p[:-1], ast_sassy_JAC2_10p[:-1], '-', color= 'cornflowerblue', linewidth = 2.0, label="Sassy_JAC_2", marker='o')
ax.plot(tput_sassy_JAC4_10p[:-1], ast_sassy_JAC4_10p[:-1], '-', color= 'lightsteelblue', linewidth = 2.0, label="Sassy_JAC_4", marker='o')
ax.tick_params(axis='both', labelsize=16)
#ax.set_xticks(np.arange(0, 8, 1))

#ax.legend(["group_ex 10%", "group_ex 100%"], loc = "center left",bbox_to_anchor=(1,0.5),fontsize=14,framealpha=0.5)
#ax.legend(["grp_ex", "grp_sh", "Sassy"], loc = "upper left",fontsize=16,framealpha=0.5)

#ax.set_title('10%', fontsize=20)
ax.set_xlim(0, 6)

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