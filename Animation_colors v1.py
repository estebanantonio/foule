#Hello world!
## Animation : Post-process the output of the Solver to create an animation ##
from numpy import *
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation



##############################################################################
############################     Parameters     ##############################
##############################################################################
input_folder = './Sim/'
output_file_name_test = input_folder+'test_line.txt'

default_save_name = input_folder+'anim_test.gif'

##############################################################################
##########################     Functions        ##############################
##############################################################################

def load_output(output_file_name = output_file_name_test):
    ''' Read the output data file and returns the time step (float),
    The position of the agents (array size=(itteration,N_agents,2)),
    The velocity of the agents (array size=(itteration,N_agents,2))'''

    ## get the time step
    file = open(output_file_name,'r')
    file.readline()
    dt = float(((file.readline()).strip()).split(' ')[-1])


    ## Get CoMPositions and velocity
    raw_data = loadtxt(output_file_name ,skiprows=2)
    N_iterations = int(raw_data[-1,0])+1 # Nb of iterations
    N_agents = int(raw_data[-1,2])+1 # Nb of agents
    # Creation of the array CoMCoMPositions and BoSPositions
    CoMPositions = zeros( (N_iterations,N_agents,2) )
    BoSPositions =  zeros( (N_iterations,N_agents,2) )
    Time=zeros( N_iterations )
    for line in raw_data:
        it, time, agent, x,y ,l_x,l_y = line
        it = int(it)
        time = float(time)
        agent = int(agent)
        Time[it]=time
        CoMPositions[it,agent,:] = array( [x,y] )
        BoSPositions[it,agent,:] = array( [l_x,l_y] )

    return Time, CoMPositions, BoSPositions


##############################################################################
#                                                                            #
#                                                                            #
#                           Animation Function                               #
#                                                                            #
#                                                                            #
##############################################################################

def create_animation(save_name = default_save_name, output_file_name = output_file_name_test):

    # Get useful values :
    Time, CoMPositions, BoSPositions = load_output(output_file_name)
    dt =mean(Time[1:]-Time[:-1]) # the animation TimeStep is set to the mean simulation value
    print('Mean dt used for the animation : ',dt)
    N_agents = len(CoMPositions[0]) # Number of agents
    N_frames = len(CoMPositions) # Number of frame to animate

    ## Creation of the Canevas :
    fig, ax = plt.subplots()
    ax.set_aspect('equal')



    R=[]
    G=[]
    for i in range(N_agents):
        R.append(0.2)
        G.append(0.3)

    # Initialize the patches that rpz the BoS
    BoS_anim = [ax.add_patch(plt.Circle((pos[0], pos[1]), 0.3, color='blue',alpha=0.5) ) for pos in BoSPositions[0]]
    # Initialize the patches that rpz the CoM
    CoM_anim = [ax.add_patch(plt.Circle((pos[0], pos[1]), 0.3, color=[R[i],G[i],0.1],alpha=1) ) for pos in CoMPositions[0]]


    Linkes = []
    for i in range (len(BoSPositions[0])):
        Linkes.append( ax.add_patch(
                        plt.Line2D([], [],
                                lw=5., ls='-', marker='o',
                                markersize=5,
                                markerfacecolor='r',
                                markeredgecolor='r',
                                alpha=0.5) ) )



    # Set the Size of the final canevas
    # Set the Size of the final canevas
    agent_radius = 0.5
    xmin = min( [ min(BoSPositions[:,:,0].flatten()),  min(CoMPositions[:,:,0].flatten()) ] )*1.2
    xmax = max( [ max(BoSPositions[:,:,0].flatten()), max(CoMPositions[:,:,0].flatten()) ]  )*1.2
    ymin = min( [ min(BoSPositions[:,:,1].flatten()), min(CoMPositions[:,:,1].flatten()) ] )*1.2
    ymax = max( [ max(BoSPositions[:,:,1].flatten()), max(CoMPositions[:,:,1].flatten()) ] )*1.2

    if abs(xmin)-1e-3 < agent_radius : xmin = -2*agent_radius
    if abs(ymin)-1e-3 < agent_radius : ymin = -2*agent_radius
    if abs(xmax)-1e-3 < agent_radius : xmax = 2*agent_radius
    if abs(ymax)-1e-3 < agent_radius : ymax = 2*agent_radius
    print("xmin : ",xmin)
    print("xmax : ",xmax)
    print("ymin : ",ymax)
    print("ymax : ",ymax)
    ax.set_xlim(xmin, xmax)
    ax.set_ylim(ymin, ymax)

    # time marker centered below the anim:
    time_text = ax.text(0.5*(xmin+xmax),ymin+0.05*abs(ymin) , 'Time:')


    Diff0=[0,0,0,0,0]
    def animate(t_step):

        #ax.patches = ax.patches[0:N_agents]
        t_step += 1

        for i in range(N_agents):


            Diff=sqrt((CoMPositions[t_step,i,0]-BoSPositions[t_step,i,0])**2 + (CoMPositions[t_step,i,1]-BoSPositions[t_step,i,1])**2)
            maxdiff = 0
            if Diff0[i]-Diff<0 and R[i] < 0.9:
                R[i]+=0.1

            elif Diff0[i]-Diff>0 and R[i]>0.1:
                R[i]-=0.1
            else:
                pass
            if maxdiff < abs(Diff0[i]-Diff):
                maxdiff = abs(Diff0[i]-Diff)
            Diff0[i]=Diff

            CoM_anim[i].set_color((R[i],G[i],0.2))

            CoM_anim[i].set_center((CoMPositions[t_step,i,0], CoMPositions[t_step,i,1]))
            BoS_anim[i].set_center((BoSPositions[t_step,i,0], BoSPositions[t_step,i,1]))

            thisx =[ CoMPositions[t_step,i,0],BoSPositions[t_step,i,0] ]
            thisy = [ CoMPositions[t_step,i,1],BoSPositions[t_step,i,1] ]
            Linkes[i].set_data(thisx,thisy)

        time_text.set_text('Time: '+str(round(dt*t_step,2))+'s')
        print(maxdiff)
        return None


    #Set the the interval between frames so the visualisation is at real speed
    interval_delay = int(dt*1000) # milliseconds between frames (must be an int)

    anim = animation.FuncAnimation(fig, animate, N_frames-1, repeat=False,
                                   interval=interval_delay)

    anim.save(save_name, writer='Pillow')
    #plt.show()
    return None

#create_animation()
def X_pos_plot():

    Time, CoMPositions, BoSPositions = load_output(output_file_name_test)
    #print("TimeSteps : ",Time[1:]-Time[:-1])
    N_agents = len(CoMPositions[0]) # Number of agents
    colors=['orange','green']
    plt.plot(
          Time,
          [x_pos for x_pos in BoSPositions[:,0,0]]
          ,'-o',label='BoS x pos',color=colors[1]
          )
    plt.plot(
          Time,
          [x_pos for x_pos in CoMPositions[:,0,0]]
          ,label='CoM x pos',color=colors[0]
          )

    for i in range(1,N_agents):
        plt.plot(
              Time,
              [x_pos for x_pos in BoSPositions[:,i,0]]
              ,'-o',color=colors[1])
        plt.plot(
              Time,
              [x_pos for x_pos in CoMPositions[:,i,0]],color=colors[0])

    plt.legend()
    plt.show()


def trajectory_plot():

    Time, CoMPositions, BoSPositions = load_output(output_file_name_test)
    print("TimeSteps : ",Time[1:]-Time[:-1])
    N_agents = len(CoMPositions[0]) # Number of agents

    plt.plot(
          [x_pos for x_pos in CoMPositions[:,0,0]],
          [y_pos for y_pos in CoMPositions[:,0,1]]
          ,label='CoM x pos')
    plt.plot(
          [x_pos for x_pos in BoSPositions[:,0,0]],
          [y_pos for y_pos in BoSPositions[:,0,1]]
          ,'-o',label='BoS x pos'
          )
    for i in range(1,N_agents):
        plt.plot(
          [x_pos for x_pos in CoMPositions[:,i,0]],
          [y_pos for y_pos in CoMPositions[:,i,1]])
        plt.plot(
          [x_pos for x_pos in BoSPositions[:,i,0]],
          [y_pos for y_pos in BoSPositions[:,i,1]]
              ,'-o')
    plt.legend()
    plt.show()
