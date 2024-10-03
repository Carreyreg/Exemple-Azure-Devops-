# Projet NS3 avec Hybrid 5G et UAVs

Ce fichier README explique comment exécuter et visualiser les codes de simulation pour un réseau hybride 5G avec des UAVs (véhicules aériens sans pilote) utilisant ns-3, Netsimulyzer et NetAnim.

## Prérequis

- Avoir installé ns-3.37
- Avoir installé Netsimulyzer pour la visualisation 3D
- Avoir installé NetAnim pour la visualisation 2D

## Étapes pour installer ns-3
1. Télécharger la version ns-3.37 et puis : 

    ```bash
       ./ns3 configure --enable-examples --enables-tests
    ```
2. 
    ```bash
       ./ns3 build 
    ```

## Étapes pour exécuter le code ns-3

1. Ouvrez un terminal et dirigez-vous vers le répertoire ns-3.

    ```bash
    cd chemin/vers/ns3
    ```

2. Lancez la simulation avec les commandes suivantes (le choix d'avoir un fichier séparé pour chaque modèle de mobilité est un choix interne) :

    ```bash
    ./ns3 run "scratch/GAUSS_MARKOV_5G.cc"
    ```
    ```bash
    ./ns3 run "scratch/MC_RANDOM_5G.cc"
    ```
    ```bash
    ./ns3 run "scratch/PAPARAZZI_5G.cc"
    ```
    ```bash
    ./ns3 run "scratch/MA_GA_5G.cc"
    ```
    ```bash
    ./ns3 run "scratch/PSO_5G.cc"
    ```
Les paramètres à modifier dans ces codes sont : 
    - `--numuavs=10`: spécifie le nombre d'UAVs
    - `--uav_relay=2`: spécifie le nombre de relais UAV (GNB)
    - `--central_frequency=28ghz`: définit la fréquence centrale à 28 GHz
    - `--sim_time=100`: fixe le temps de simulation à 100 secondes ou autres

## Visualisation des résultats

### Visualisation 3D avec Netsimulyzer

Le code de simulation 3D se trouve dans un autre fichier code : "scratch/Netsimulyzer_simulation.cc"
1. Lancez le code de simulation avec la commande suivante :
   
   ```bash
    ./ns3 run scratch/Netsimulyzer_simulation.cc
   ```

2. Dirigez-vous vers le répertoire de Netsimulyzer.
    ```bash
    cd chemin/vers/netsimulyzer
    ```
2. Lancez Netsimulyzer.

    ```bash
    ./build/netsimulyzer
    ```

3. Une fenêtre apparaîtra. Utilisez l'option "Load" pour charger le fichier JSON généré par le code de simulation ns-3.
4. Un fichier de mobilité "netsimulyzer_mobility.txt" de test a été utilisé dans le code "scratch/Netsimulyzer_simulation.cc", il peut étre modifier dans la ligne 45

### Visualisation 2D avec NetAnim

1. Dirigez-vous vers le répertoire de NetAnim.

    ```bash
    cd chemin/vers/netanim
    ```

2. Lancez NetAnim.

    ```bash
    ./NetAnim
    ```

3. Une fenêtre apparaîtra. Utilisez l'option "Load" pour charger le fichier XML généré par le code ns-3.

## Fichiers en sortie

- Un fichier JSON pour la visualisation 3D avec Netsimulyzer
- Un fichier XML pour la visualisation 2D avec NetAnim

## Auteurs

Abdelmadjid AIT SAADI

## Licence

Ce projet est sous licence.