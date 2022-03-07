#https://python.doctor/page-tkinter-interface-graphique-python-tutoriel
#http://tkinter.fdex.eu/doc/
# 
## Animation : Post-process the output of the Solver to create an animation ##
from numpy import *
import time
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import moviepy.editor as mp
from tkinter import * 
import threading
from PIL import Image, ImageTk 
import imageio

##############################################################################
############################     Parameters     ##############################
##############################################################################
input_folder = './Sim/'
default_file = input_folder+'test_line.txt'

default_save_name = input_folder+'anim_test.gif'

##############################################################################
##########################     Functions        ##############################
##############################################################################

def load_output(output_file_name):
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

def create_animation(save_name, output_file_name):

    # Get useful values :
    Time, CoMPositions, BoSPositions = load_output(output_file_name)
    dt =mean(Time[1:]-Time[:-1]) # the animation TimeStep is set to the mean simulation value
    print('Mean dt used for the animation : ',dt)
    N_agents = len(CoMPositions[0]) # Number of agents
    N_frames = len(CoMPositions) # Number of frame to animate

    ## Creation of the Canevas :
    fig, ax = plt.subplots()
    ax.set_aspect('equal')


    R1=[]
    G1=[]
    B1=[]
    R=[]
    G=[]
    for i in range(N_agents):
        R1.append(random.rand())
        G1.append(random.rand())
        B1.append(random.rand())
        R.append(0)
        G.append(0.3)

    # Initialize the patches that rpz the BoS
    BoS_anim = [ax.add_patch(plt.Circle((pos[0], pos[1]), 0.3, color='blue',alpha=0.5) ) for pos in BoSPositions[0]]
    # Initialize the patches that rpz the CoM
    CoM_anim = [ax.add_patch(plt.Circle((pos[0], pos[1]), 0.3, color=[R[i],G[i],0.1],alpha=1) ) for pos in CoMPositions[0]]
    CoM_animAgent = [ax.add_patch(plt.Circle((pos[0], pos[1]), 0.15, color=[R1[i],G1[i],B1[i]],alpha=0.7) ) for pos in CoMPositions[0]]

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
    xmin = min( [ min(BoSPositions[:,:,0].flatten()),  min(CoMPositions[:,:,0].flatten()) ] )*1.2 - agent_radius
    xmax = max( [ max(BoSPositions[:,:,0].flatten()), max(CoMPositions[:,:,0].flatten()) ]  )*1.2 + agent_radius
    ymin = min( [ min(BoSPositions[:,:,1].flatten()), min(CoMPositions[:,:,1].flatten()) ] )*1.2 - agent_radius
    ymax = max( [ max(BoSPositions[:,:,1].flatten()), max(CoMPositions[:,:,1].flatten()) ] )*1.2 + agent_radius

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
    maxdiff = 0
    for i in range(N_frames):
        for j in range(N_agents):
            Diff=sqrt((CoMPositions[i,j,0]-BoSPositions[i,j,0])**2 + (CoMPositions[i,j,1]-BoSPositions[i,j,1])**2)
            if maxdiff < Diff : 
                maxdiff = Diff
    print(maxdiff)
    def animate(t_step):

        #ax.patches = ax.patches[0:N_agents]
        t_step += 1

        for i in range(N_agents):


            Diff=sqrt((CoMPositions[t_step,i,0]-BoSPositions[t_step,i,0])**2 + (CoMPositions[t_step,i,1]-BoSPositions[t_step,i,1])**2)

            
            R[i] = Diff/maxdiff
            if R[i] < 0:
                R[i]=0
            elif R[i]>1:
                R[i]=1
            else:
                pass
            CoM_anim[i].set_color((R[i],G[i],0.2))
            CoM_animAgent[i].set_color((R1[i],G1[i],B1[i]))
            CoM_animAgent[i].set_center((CoMPositions[t_step,i,0], CoMPositions[t_step,i,1]))
            CoM_anim[i].set_center((CoMPositions[t_step,i,0], CoMPositions[t_step,i,1]))
            BoS_anim[i].set_center((BoSPositions[t_step,i,0], BoSPositions[t_step,i,1]))

            thisx =[ CoMPositions[t_step,i,0],BoSPositions[t_step,i,0] ]
            thisy = [ CoMPositions[t_step,i,1],BoSPositions[t_step,i,1] ]
            Linkes[i].set_data(thisx,thisy)

        time_text.set_text('Time: '+str(round(dt*t_step,2))+'s')
        
        return None


    #Set the the interval between frames so the visualisation is at real speed
    interval_delay = int(dt*1000) # milliseconds between frames (must be an int)

    anim = animation.FuncAnimation(fig, animate, N_frames-1, repeat=False,
                                   interval=interval_delay)

    anim.save(save_name, writer='Pillow')
    #clip = mp.VideoFileClip(save_name)
    #clip.write_videofile("resultat.mp4")
    #plt.show()
    return None
    
    
    
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

    
    
    def animate(t_step):

        #ax.patches = ax.patches[0:N_agents]
        #t_step += 1
        for i in range(N_agents):
            
            CoM_anim[i].set_center((CoMPositions[t_step,i,0], CoMPositions[t_step,i,1]))
            BoS_anim[i].set_center((BoSPositions[t_step,i,0], BoSPositions[t_step,i,1]))
            
            thisx =[ CoMPositions[t_step,i,0],BoSPositions[t_step,i,0] ]
            thisy = [ CoMPositions[t_step,i,1],BoSPositions[t_step,i,1] ] 
            Linkes[i].set_data(thisx,thisy)
            
        time_text.set_text('Time: '+str(round(dt*t_step,2))+'s')
        return None
    
    for i in range(0,N_frames) :
        #plt.savefig(str(i)+".png")
        animate(i)
    
    """
    #Set the the interval between frames so the visualisation is at real speed
    interval_delay = int(dt*1000) # milliseconds between frames (must be an int)
    
    anim = animation.FuncAnimation(fig, animate, N_frames-1, repeat=False,
                                   interval=interval_delay)
    
    anim.save(save_name, writer='Pillow')"""
    #plt.show()
    return None

#create_animation()
def X_pos_plot(default_file_name):
    plt.clf()
    Time, CoMPositions, BoSPositions = load_output(default_file_name)
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

    plt.savefig("Xposplot.png")
    plt.show()
    plt.close()
    
  
def trajectory_plot(default_file_name):
    plt.clf()
    Time, CoMPositions, BoSPositions = load_output(default_file_name)
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
    plt.savefig("trajectoryplot.png")
    plt.show()
    plt.close()
##############################################################################
############################     Tkinter        ##############################
##############################################################################

def interface1() :
    """afficherAnimation()
    afficherXPlot()
    afficherTrajPlot()"""
    
    canvas = Canvas(fenetre, width=250, height=100, bg='ivory')
    canvas.pack(side=TOP, padx=5, pady=5)
    txt = canvas.create_text(125, 50, text="Importez un fichier", font="Arial 16 italic", fill="black")
    global value
    
    # entrée
    value = StringVar() 
    value.set(default_file)    
    entree = Entry(fenetre, textvariable=value, width=30)
    entree.pack(side=BOTTOM, pady=5)
    
    button = Button(fenetre, text ='Importer', command = importerClicked)
    button.pack(side=RIGHT, padx=5, pady=5)


def importerClicked() :
    print(value.get())
    create_animation("tet.gif", "./Sim/test_line.txt") #default_save_name
    print("finished animation")
    trajectory_plot("./Sim/test_line.txt")
    print("finished traj")
    X_pos_plot("./Sim/test_line.txt")
    print("finished xplot")
    
    for widget in fenetre.winfo_children():
        widget.destroy()
    interface2()
    print("done")
    
def interface2() :
    global canvas2
    global label
    label = Label(fenetre)
    canvas2 = Canvas(fenetre, width=250, height=100, bg='ivory')
    canvas2.pack(side=TOP, padx=5, pady=5)
    button1 = Button(fenetre, text ='Animation',command = afficherAnimation).pack(side=BOTTOM)
    button2 = Button(fenetre, text ='Xposplot',command = afficherXPlot).pack(side=BOTTOM)
    button3 = Button(fenetre, text ='Trajectoryplot',command = afficherTrajPlot).pack(side=BOTTOM)


def afficherXPlot():
    #canvas = Canvas(fenetre)    
    label.pack_forget()
    image = Image.open("Xposplot.png") 
    photo = ImageTk.PhotoImage(image) 
 
    canvas2.configure(width = image.size[0], height = image.size[1])
    canvas2.create_image(0,0, anchor = NW, image=photo)
    canvas2.pack() 
    fenetre.mainloop()

    
def afficherTrajPlot():
    #canvas = Canvas(fenetre)
    label.pack_forget()
    image = Image.open("trajectoryplot.png") 
    photo = ImageTk.PhotoImage(image) 
 
    canvas2.configure(width = image.size[0], height = image.size[1])
    canvas2.create_image(0,0, anchor = NW, image=photo)
    canvas2.pack() 
    
    fenetre.mainloop()

def lecturevideo(label):

    for image in video.iter_data():
        frame_image = ImageTk.PhotoImage(Image.fromarray(image))
        label.config(image=frame_image)
        label.image = frame_image
        time.sleep(0.01)
        
def afficherAnimation():
    #label = Label(fenetre)
    canvas2.pack_forget()
    global video
    video_name = "resultat.mp4" #This is your video file path
    video = imageio.get_reader(video_name)
    label.pack()
    thread = threading.Thread(target=lecturevideo, args=(label,))
    thread.daemon = 1
    thread.start()
    fenetre.mainloop()
    
    
fenetre = Tk()

interface1()



fenetre.mainloop()
