import random
import math
import numpy as np

def euclidean_distance(x1, y1, x2, y2):
    distance = math.sqrt((x2 - x1)**2 + (y2 - y1)**2)
    return distance


def deplacement_uav_relais(coordonnees, source, distance_k):
     
    coordonnees_array = np.array(coordonnees)
    position_moyenne = np.mean(coordonnees_array, axis=0)
    vecteur_deplacement = position_moyenne - source
    norme_vecteur_deplacement = np.linalg.norm(vecteur_deplacement)
    direction = vecteur_deplacement / norme_vecteur_deplacement    
    nouvelle_position_source = position_moyenne - direction * distance_k
    return nouvelle_position_source



def is_close_to_target(targets, coordinate, desired_distance):
    result = []
    if euclidean_distance(coordinate[0],coordinate[1],targets[0],targets[1]) < desired_distance:
        return True
    else:
        return False

def generate_targets(num_targets,bounds):
    list=[]
    for i in range(num_targets):
        x =((bounds[0][0]+i*bounds[0][1]/num_targets)+(i+1)*bounds[0][1]/num_targets)/2
        y =((bounds[0][0]+i*bounds[0][1]/num_targets)+(i+1)*bounds[0][1]/num_targets)/2
        list.append([x,y])
    return list


def divide_bounds(num_agents,bounds):
    result_list=[]
    for i in range(num_agents):
        xmax = (i+1)*(bounds[0][1]-bounds[0][0])/(num_agents)
        xmin = bounds[0][0]+(i)*(bounds[0][1]-bounds[0][0])/(num_agents)
        ymax = (i+1)*(bounds[0][1]-bounds[0][0])/(num_agents)
        ymin = bounds[0][0]+(i)*(bounds[0][1]-bounds[0][0])/(num_agents)
        result_list.append([(xmin,xmax),(ymin,ymax)])
    return result_list



def generate_coordinates(num_cluster,num_cor,num_drones, min_coord_x, max_coord_x, min_coord_y, max_coord_y,output_file,targets):
    coordinates = []
    stop_drones = []
    liste=[]
    #remplissage liste des coordonnées
    for i in range(num_cluster):
        liste.append([])
        stop_drones.append([])
    t=0
    liste_b=divide_bounds(num_cluster,[(min_coord_x,max_coord_x),(min_coord_y,max_coord_y)])
    for i in range(num_drones):
        stop_drones[i//(num_drones//num_cluster)].append(0)
    print(stop_drones)
    with open(output_file, 'w') as file:
        for j in range(num_cor):
            #t=float(j/10)
            for z in range(num_cluster):
                for i in range(num_drones//num_cluster): 
                    x = random.uniform(liste_b[z][0][0],liste_b[z][0][1])
                    y = random.uniform(liste_b[z][1][0], liste_b[z][1][1])
                    coordinate=[x,y]
                    if(j==0):
                        liste[z].append(coordinate)
                        file.write(f'$ns_ at {j} "$node_({i+num_cluster+(z*num_drones//num_cluster)}) setdest {x:.2f} {y:.2f} 5.0 "\n')
                    else:
                        if(stop_drones[z][i] == 0):
                            liste[z][i]=coordinate
                            if (is_close_to_target(targets[z],coordinate,5)):
                                stop_drones[z][i]=1
                                file.write(f'$ns_ at {j} "$node_({i+num_cluster+(z*num_drones//num_cluster)}) setdest {x:.2f} {y:.2f} 5.0 "\n')
                            else:
                                file.write(f'$ns_ at {j} "$node_({i+num_cluster+(z*num_drones//num_cluster)}) setdest {x:.2f} {y:.2f} 5.0 "\n')
                relai=deplacement_uav_relais(liste[z],[0,0],2.0)
                print(f"Le relais {z} est dans : {relai}")
                if(j==0):
                        file.write(f'$node_({z}) set X_ {relai[0]:.2f} \n')
                        file.write(f'$node_({z}) set Y_ {relai[1]:.2f} \n')
                else:
                    file.write(f'$ns_ at {j} "$node_({z}) setdest {relai[0]:.2f} {relai[1]:.2f} 50.0 "\n')
                
            #print(stop_drones)
    return coordinates

# Il faut concorder les parametres du code aux parametres de simulation "ou modifier pour qu'il puissent etre reutilisable (parametre en argument de cli)"
min_coord_y=0
max_coord_y=100
num_drones = 6
num_cor= 100
min_coord_x = 0
max_coord_x = 100
num_cluster=2

targets=generate_targets(num_cluster,[(min_coord_x,max_coord_x),(min_coord_y,max_coord_y)])

coordinates = generate_coordinates(num_cluster,num_cor,num_drones, min_coord_x, max_coord_x,min_coord_y,max_coord_y,"trace_drone.txt",targets)