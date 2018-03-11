import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
import gflags  # sudo pip install python-gflags
import sys
import scipy as sp # sudo pip install spicy
from scipy import stats
import math



#gflags.DEFINE_string('cris', None, 'un parametro para testear', short_name='c')
gflags.DEFINE_string('size', None, 'The model size', short_name='s')
gflags.DEFINE_list('device_name', None, 'Broken device name', short_name='d') # pass devices name separated with ,
#gflags.DEFINE_string('size', 149, 'The model size', short_name='s')
gflags.MarkFlagAsRequired('size')
gflags.MarkFlagAsRequired('device_name')

FLAGS = gflags.FLAGS

try:
    argv = FLAGS(sys.argv)  # parse flags
except gflags.FlagsError, e:
    print '%s\nUsage: %s ARGS\n%s' % (e, sys.argv[0], FLAGS)
    sys.exit(1)

#print "anduvo", FLAGS.size

df_Command = pd.DataFrame()
df_Devices = pd.DataFrame()
df_Task = pd.DataFrame()
for prob in range(10, 100, 10):
    nameCommand = "datafiles/ModelSize"+str(FLAGS.size)+"devices"+str(len(FLAGS.device_name))
    nameTask = "datafiles/ModelSize"+str(FLAGS.size)+"devices"+str(len(FLAGS.device_name))
    nameDevices = "datafiles/ModelSize"+str(FLAGS.size)+"devices"+str(len(FLAGS.device_name))
    for i in range(len(FLAGS.device_name)):
    	nameCommand += "_"+str(prob)+str(FLAGS.device_name[i])
    	nameTask += "_"+str(prob)+str(FLAGS.device_name[i])
    	nameDevices += "_"+str(prob)+str(FLAGS.device_name[i])
    nameCommand += "command.txt"
    nameTask += "task.txt"
    nameDevices += "device.txt"
    #print nameCommand
    #print nameTask
    #print nameDevices
    df_new_Command = pd.read_csv(nameCommand, sep=" ")
    df_new_Command['probability'] = [prob for i in range(len(df_new_Command))]
    df_Command = pd.concat([df_Command, df_new_Command])
    df_new_Devices = pd.read_csv(nameDevices, sep=" ")
    df_new_Devices['probability'] = [prob for i in range(len(df_new_Devices))]
    df_Devices = pd.concat([df_Devices, df_new_Devices])
    df_new_Task = pd.read_csv(nameTask, sep=" ")
    df_new_Task['probability'] = [prob for i in range(len(df_new_Task))]
    df_Task = pd.concat([df_Task, df_new_Task])

#print df_Command
#print df_Devices
#print df_Task

###COMMANDS RECEIVED PER PERSON
CoomandNames = map(lambda x: str(x), df_Command.groupby(['Command']).indices.keys())

for Command in range(len(CoomandNames)):
	df_data = pd.DataFrame()
	data = []
	labels = []
	xtitle = ""
 	command_temp = CoomandNames[Command]
 	conf_interval = []
 	sample_size = []
 	data_mean = []
 	data_std = []
 	#z_critical = stats.norm.ppf(q = 0.995)  # Get the z-critical value* for 99%
 	z_critical = stats.norm.ppf(q = 0.975)  # Get the z-critical value* for 99%
	print("z-critical value:" + str(z_critical))              # Check the z-critical value                     

	for prob in range(10, 100, 10):
		df_data[["#Persons"+str(prob),"Command"+str(prob)]] = df_Command[df_Command["probability"] == prob][["#Persons","Command"]]
		data.append(df_data[df_data["Command"+str(prob)] == command_temp]["#Persons"+str(prob)])		  
		labels.append(prob)
		sample_size.append(len(df_data[df_data["Command"+str(prob)] == command_temp]["#Persons"+str(prob)]))
		data_mean.append((df_data[df_data["Command"+str(prob)] == command_temp]["#Persons"+str(prob)]).mean())
		data_std.append((df_data[df_data["Command"+str(prob)] == command_temp]["#Persons"+str(prob)]).std())
		#print "sample size: "
		#print sample_size
	for i in range(len(data_mean)):
		#print "data i:" +str(data_mean[i]) + " " + str(data_std[i])
		margin_of_error = z_critical * (data_std[i]/math.sqrt(sample_size[i]))
		conf = [data_mean[i] - margin_of_error, data_mean[i] + margin_of_error]
		conf_interval.append(conf)
	print conf_interval
	#print data
	plt.boxplot(data, labels = labels, showmeans = True, notch = True, conf_intervals=conf_interval)
	plt.suptitle(command_temp, fontsize=14, fontweight='bold')
	for i in range(len(FLAGS.device_name)):
 		xtitle += " " +str(FLAGS.device_name[i])
	plt.xlabel("Failure probability of devices:" +xtitle, fontsize=12)
	plt.ylabel('Total number of persons that receives the command', fontsize=12)
	plt.ylim(bottom=-1)
	#plt.show()
	for i in range(len(FLAGS.device_name)):
		figuretitle = '_'+str(FLAGS.device_name[i])
	plt.savefig(str(FLAGS.size)+'BoxPlot_Command_IC'+command_temp+figuretitle+'.png')
	plt.close()


# ###USE OF DEVICES
# 
DevicesNames = map(lambda x: str(x), df_Devices.groupby(['Device']).indices.keys())
for Device in range(len(DevicesNames)):
 	device_temp = DevicesNames[Device]
 	#print device_temp
	df_data = pd.DataFrame()
	data = []
	labels = []
	xtitle = ""
	for prob in range(10, 100, 10):
		df_data[["#Uses"+str(prob),"Device"+str(prob)]] = df_Devices[df_Devices["probability"] == prob][["#Uses","Device"]]
		data.append(df_data[df_data["Device"+str(prob)] == device_temp]["#Uses"+str(prob)])
		labels.append(prob)
	#print data
	plt.boxplot(data, labels = labels, showmeans = True)
	plt.suptitle(device_temp, fontsize=14, fontweight='bold')
	for i in range(len(FLAGS.device_name)):
 		xtitle += " " +str(FLAGS.device_name[i])
	plt.xlabel("Failure probability of devices:" +xtitle, fontsize=12)
	plt.ylabel('Total number of uses of the device', fontsize=12)
	plt.ylim(bottom=-1)
	#plt.show()
	for i in range(len(FLAGS.device_name)):
		figuretitle = '_'+str(FLAGS.device_name[i])
	plt.savefig(str(FLAGS.size)+'BoxPlot_Device_'+device_temp+figuretitle+'.png')
	plt.close()
