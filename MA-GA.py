from __future__ import division
import numpy as np
import random
import math
import numpy as np


# Constantes
NUM_AGENTS=2
POPULATION_SIZE = 10
Bounds = [(0,100),(0,100)]
MAX_GENERATIONS = 5
MUTATION_RATE = 0.4
VITESSE_LIMIT=2
output_file="maga_trace.txt"




with open(output_file, 'w') as file:
    pass  # Pour vider le fichier


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


def calculate_velocity(pos1,pos2):
    vel_x=abs(pos2[0]-pos1[0])
    vel_y=abs(pos2[1]-pos1[1])
    return vel_x+vel_y


def dispersion_inividu(num_individu,bounds):
    result_list=[]
    for i in range(num_individu):
        #x = random.uniform(bounds[0][0]+(i)*(bounds[0][1]-bounds[0][0])/(num_individu), (i+1)*(bounds[0][1]-bounds[0][0])/(num_individu))
        #y = random.uniform(bounds[0][0]+(i)*(bounds[0][1]-bounds[0][0])/(num_individu), (i+1)*(bounds[0][1]-bounds[0][0])/(num_individu))
        x=random.uniform(bounds[0][0],bounds[0][1])
        y=random.uniform(bounds[1][0],bounds[1][1])
        result_list.append([x,y])
    return result_list


def divide_population(num_agents,bounds):
    result_list=[]
    for i in range(num_agents):
        xmax = (i+1)*(bounds[0][1]-bounds[0][0])/(num_agents)
        xmin = bounds[0][0]+(i)*(bounds[0][1]-bounds[0][0])/(num_agents)
        ymax = (i+1)*(bounds[0][1]-bounds[0][0])/(num_agents)
        ymin = bounds[0][0]+(i)*(bounds[0][1]-bounds[0][0])/(num_agents)
        result_list.append([(xmin,xmax),(ymin,ymax)])
    return result_list





class UAV(object):
    def __init__(self):
        self.batterie = 100
        
    def update_battery(self,velocity):
        energy_per_distance = 0.1  # J/m
        if(self.batterie-abs(energy_per_distance*velocity)<=0):
            self.batterie=0
        else:
            self.batterie=self.batterie-abs(energy_per_distance*velocity)



class Individu(object):
    def __init__(self, genes, fitness=None):
        self.genes_xy = [genes[i] for i in range(len(genes))]
        self.fitness = fitness
        self.batterie = 100
        
    

def initialize_population(pop_size,bounds,target):
    list=dispersion_inividu(pop_size,bounds)
    population=[]
    for i in range(pop_size):
        individual = Individu(list[i],func1(list[i],target))
        population.append(individual)
    return population

def func1(position,target):
    fitness=euclidean_distance(position[0],position[1],target[0],target[1])
    return fitness


def calculate_fitness(individual,target):
    fitness=euclidean_distance(individual.genes_xy[0],individual.genes_xy[1],target[0],target[1])
    return fitness



def selection(pop_size,population,target):
    selected = []
    for _ in range(pop_size):
        tournament = random.sample(population, 2)
        #best_individual = min(tournament, key=calculate_fitness)
        best_individual = min(tournament,key=lambda individual: calculate_fitness(individual, target))
        selected.append(best_individual)
    return selected


def crossover(parent1, parent2,target):
    pos1=[-1,-1]
    pos2=[-1,-1]
    r1 = random.random()
    r2 = random.random()
    for i in range(2):
        dec1= r1/8*(target[i]-parent1.genes_xy[i])
        dec2= r2/8*(target[i]-parent2.genes_xy[i])
        if( dec1 < 2 ):
            pos1[i] = parent1.genes_xy[i] + dec1
        else:
            pos1[i] = parent1.genes_xy[i] + 2
        if (dec2 < 2):
            pos2[i] = parent2.genes_xy[i] + dec2
        else:
            pos2[i] = parent2.genes_xy[i] + 2
            
    
    child1=Individu(pos1,func1(pos1,target))
    child2=Individu(pos2,func1(pos2,target))
    
    return child1, child2

def euclidean_distance(x1, y1, x2, y2):
    distance = math.sqrt((x2 - x1)**2 + (y2 - y1)**2)
    return distance


def mutation(individual):
    if random.random() < MUTATION_RATE:
        tmp=individual.genes_xy[0]
        individual.genes_xy[0]=individual.genes_xy[1]
        individual.genes_xy[1]=tmp
    return individual


def replace_population(population, offspring):
    population = random.sample(population, len(population) - len(offspring))
    population.extend(offspring)
    return population

# Algorithme genetique
def genetic_algorithm(population_size,bounds,target,agent):
    nodes_position=[]
    for i in range(population_size):
        nodes_position.append([])
    #création de la population
    population = initialize_population(population_size,bounds,target)
   # print(population)
    #initialisation de la meilleure fitness avec l'infini
    best_fitness = float('inf')
    #initialisation du meilleure individu
    best_individual = None
    #la boucle de la génération de la population
    cpt=0
    with open(output_file, 'a') as file:
        for generation in range(MAX_GENERATIONS):
            # mettre les fitness de chaque individu dans une liste
            fitness_values = [individual.fitness for individual in population]
            #trouver le meilleur individu de la population
            min_fitness = min(fitness_values)
            if min_fitness < best_fitness:
                best_fitness = min_fitness
                best_individual = population[fitness_values.index(min_fitness)]
            # selection des prarent pour la reproduction
            selected_population = selection(population_size,population,target)

            # Creer les childs
            offspring = []
            for i in range(population_size // 2):
                parent1, parent2 = random.sample(selected_population, 2)
                child1, child2 = crossover(parent1, parent2,target)
                offspring.append(mutation(child1))
                offspring.append(mutation(child2))
            # remplacer quelques individu avec les nouveau individu generer

            for i in range(len(population)):
                print(f"L individu{i} est dans :",population[i].genes_xy)
                file.write(f'$ns_ at {cpt} "$node_({i+NUM_AGENTS+(agent*len(population))}) setdest {population[i].genes_xy[0]:.2f} {population[i].genes_xy[1]:.2f} 50.0 "\n')
                nodes_position[i].append(population[i].genes_xy)
            liste=[]
            for x in range(0,len(population)):
                liste.append(population[x].genes_xy)
            relai=deplacement_uav_relais(liste,[0,0],2.0)
            print(f"Le relais {agent} est dans : {relai}")
            if(cpt==0):
                file.write(f'$node_({agent}) set X_ {relai[0]:.2f} \n')
                file.write(f'$node_({agent}) set Y_ {relai[1]:.2f} \n')
            else:
                file.write(f'$ns_ at {cpt} "$node_({agent}) setdest {relai[0]:.2f} {relai[1]:.2f} 50.0 "\n')
            liste.clear()
            cpt=cpt+1
            population = replace_population(population, offspring)
    return nodes_position

# Example execution

sub_population=int(POPULATION_SIZE/NUM_AGENTS)

bound_i = divide_population(NUM_AGENTS,Bounds)

target_locations = generate_targets(NUM_AGENTS,Bounds)

nodes=[]
for i in range(NUM_AGENTS):
    nodes=nodes+genetic_algorithm(sub_population,bound_i[i],target_locations[i],i)
    print("-------------------------------------------------------------------------------------")







