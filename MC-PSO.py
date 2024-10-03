from __future__ import division
import random
import math
import numpy as np
import csv


#--- les fonctions utiles -------------------------------------------------------------------------+

MAX_SPEED=2
def deplacement_uav_relais(coordonnees, source, distance_k):
     
    coordonnees_array = np.array(coordonnees)
    position_moyenne = np.mean(coordonnees_array, axis=0)
    vecteur_deplacement = position_moyenne - source
    norme_vecteur_deplacement = np.linalg.norm(vecteur_deplacement)
    direction = vecteur_deplacement / norme_vecteur_deplacement    
    nouvelle_position_source = position_moyenne - direction * distance_k
    return nouvelle_position_source

def generate_targets(num_targets,bounds):
    list=[]
    for i in range(num_targets):
        x =((bounds[0][0]+i*bounds[0][1]/num_targets)+(i+1)*bounds[0][1]/num_targets)/2
        y =((bounds[0][0]+i*bounds[0][1]/num_targets)+(i+1)*bounds[0][1]/num_targets)/2
        list.append([x,y])
    return list



def dispersion_particles(num_particles,bounds):
    list=[]
    for i in range(num_particles):
        x =round(random.uniform(bounds[0][0]+i*(bounds[0][1]-bounds[0][0])/(num_particles), (i+1)*(bounds[0][1]-bounds[0][0])/(num_particles)),2) 
        y =round(random.uniform(bounds[0][0]+i*(bounds[0][1]-bounds[0][0])/(num_particles), (i+1)*(bounds[0][1]-bounds[0][0])/(num_particles)),2) 
        list.append([x,y])
    return list


# fonction fitness qui attribut une note à la solution (distance de la target= moindre cout)


def euclidean_distance(x1, y1, x2, y2):
    distance = math.sqrt((x2 - x1)**2 + (y2 - y1)**2)
    return distance


# fonction fitness qui attribut une note à la solution (distance de la target= moindre cout)

def func1(position):
    target_locations =generate_targets(5,[(0,100),(0,100)])
    mini=4000    
    for target in target_locations:
        if euclidean_distance(position[0],position[1],target[0],target[1]) < mini:
            mini=euclidean_distance(position[0],position[1],target[0],target[1])
    fitness=mini
   # print(fitness)
    return fitness



#--- MAIN ---------------------------------------------------------------------+

class Particle:
    def __init__(self,x0,cluster):
        self.position_i=[]          # particle position
        self.velocity_i=[]          # particle velocity
        self.pos_best_i=[]          # best position individual
        self.err_best_i=-1          # best error individual
        self.err_i=-1               # error individual
        self.cluster=-1
        self.batterie=100

        for i in range(0,2):
            self.velocity_i.append(round(random.uniform(-1,1),2)) #velocité random à la création 
            self.position_i.append(x0[i])
        self.cluster=cluster                             #definir le cluster
    
    

    # evaluation de la fitness
    def evaluate(self,costFunc):
        self.err_i=costFunc(self.position_i) #on calcule la fonction fitness
        # la position actuelle est elle une best perso ?
        if self.err_i < self.err_best_i or self.err_best_i==-1: # self.err_best_i==-1 c'est pour prendre la premiere fitness(apres la création )
            self.pos_best_i=self.position_i 
            self.err_best_i=self.err_i

    def update_battery(self,velocity):
        energy_per_distance = 0.1  # J/m
        if(self.batterie-abs(energy_per_distance*velocity)<=0):
            self.batterie=0
        else:
            self.batterie=self.batterie-abs(energy_per_distance*velocity)
        
    
    
    # mise à jour de la vélocité
    def update_velocity(self,pos_best_g):
        w=0.5     # constante d'inertie (combien on pondere l'ancienne velocité)
        c1=0.7   #  constante pour la solution perso
        c2=2.2       # constante pour la solution de cluster
        for i in range(0,2):
            self.update_battery(self.velocity_i[i])
            r1=round(random.random(),2)
            r2=round(random.random(),2)
            #mise à jour du niveau de la batterie avant de mettre à jour la velocité
            vel_cognitive=c1*r1*(self.pos_best_i[i]-self.position_i[i])
            vel_social=c2*r2*(pos_best_g[i]-self.position_i[i])
            vel=w*self.velocity_i[i]+vel_cognitive+vel_social
            if(vel>MAX_SPEED):
                self.velocity_i[i]=MAX_SPEED
            elif (vel<(-1)*MAX_SPEED):
                self.velocity_i[i]=(-1)*MAX_SPEED
            else:
                self.velocity_i[i]=vel
            
    # mise à jour de la position avec la nouvelle velocité
    def update_position(self,bounds):
        for i in range(0,2):
            self.position_i[i]=self.position_i[i]+self.velocity_i[i]
            # pour rester toujours dans la zone
            if self.position_i[i]>bounds[i][1]:
                self.position_i[i]=bounds[i][1]

            # ajuster la position en cas ou
            if self.position_i[i] < bounds[i][0]:
                self.position_i[i]=bounds[i][0] 
        
        
class PSO():
    def __init__(self,costFunc,x0,bounds,num_particles,maxiter,num_cluster,output_file):
        
        relay_position=[]
        NUM_RELAYS=num_cluster
        batterie_particules=[]
        err_best_cluster=[]                 # meilleure erreur dans le cluster
        pos_best_cluster=[]                 # meilleurs position dans le cluster
        cluster=[]
        position_particles=[]
        for b in range(num_cluster):
            err_best_cluster.append(int(-1))                   # meilleure erreur dans le cluster
            pos_best_cluster.append([])
            
        for e in range(num_particles):
            position_particles.append([])
            batterie_particules.append([])

        # création de l'essaim
        swarm=[]
        j=0
        for i in range(0,num_particles):
            swarm.append(Particle(x0[i],j))
            if(((i+1)%int(num_particles/num_cluster))==0):
                err_best_cluster[j]=-1
                pos_best_cluster[j]=[]
                j=j+1
        # boucle d'optimisation
        i=0
        with open(output_file, 'w') as file:
            while i < maxiter:
                # parcourir les particules et evaluation des fitness
                z=0 #pour iterer les cluster
                for j in range(0,num_particles):
                    swarm[j].evaluate(costFunc)
                        # determiner si cette particule est la meilleure (globalement dans le cluster )
                    if swarm[j].err_i < err_best_cluster[z] or err_best_cluster[z] == -1:     
                        pos_best_cluster[z]=list(swarm[j].position_i)
                        err_best_cluster[z]=float(swarm[j].err_i)
                    if(((j+1)%int(num_particles/num_cluster))==0):
                        z=z+1
            # mettre à jour les velo et pos
                z=0
               # print(position_particles[0])
                for j in range(0,num_particles):
                    swarm[j].update_velocity(pos_best_cluster[z])
                    swarm[j].update_position(bounds)
                    if(((j+1)%int(num_particles/num_cluster))==0):
                        z=z+1    
                    print(f"La particule {j+NUM_RELAYS} est dans : [{swarm[j].position_i[0]:.2f},{swarm[j].position_i[1]:.2f}]")
                    #print(f"{swarm[j].err_best_i:.2f}")
                    file.write(f'$ns_ at {i} "$node_({j+NUM_RELAYS}) setdest {swarm[j].position_i[0]:.2f} {swarm[j].position_i[1]:.2f} 50.0 "\n')
                    pos=list(swarm[j].position_i)
                    bat=swarm[j].batterie
                    position_particles[j].append(pos)
                    batterie_particules[j].append(bat)
                #Mise à jour de la position des noeuds de relais
                liste=[]
                for c in range(0,num_cluster):
                    for x in range(0,num_particles):
                        if swarm[x].cluster==c:
                            liste.append(swarm[x].position_i)
                    relai=deplacement_uav_relais(liste,[0,0],2.0)
                    print(f"Le relais {c} est dans : {relai}")
                    if(i==0):
                            file.write(f'$node_({c}) set X_ {relai[0]:.2f} \n')
                            file.write(f'$node_({c}) set Y_ {relai[1]:.2f} \n')
                    else:
                        file.write(f'$ns_ at {i} "$node_({c}) setdest {relai[0]:.2f} {relai[1]:.2f} 50.0 "\n')
                    liste.clear()
                i+=1
            print ('FINAL:')
   

#--- Exécution ----------------------------------------------------------------------+

# Il faut concorder les parametres du code aux parametres de simulation "ou modifier pour qu'il puissent etre reutilisable (parametre en argument de cli)"
bounds=[(0,100),(0,100)]  # les limites
positions_initales=dispersion_particles(10,bounds)
PSO(func1,positions_initales,bounds,num_particles=10,maxiter=25,num_cluster=2,output_file="./pso_trace.txt")
