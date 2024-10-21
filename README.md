# CG-2021
Trabalho realizado juntamente com @luislcc, @flake-png e @monteiro06 para a Unidade curricular de Computação Gráfica, o Projeto consistia de uma engine em C++ utilizando-se de OpenGl que deveria ler arquivos em XML com as Scenes a serem desenhadas, e um gerador de Shapes 3D também em C++ que contava com shapes simples desde cubos a tórus e com leitura de arquivos de patches, para criação de formas complexas utilizando de curvas de Bezier.

A Engine desenvolvida conta ainda com um interpretador de uma linguagem emulada em XML para facilitar o desenvolvimento de Scenes mais complexas. O interpretador conta com comandos básicos descritos em tags no XML como if then else e while loop, execução de aritmética básica e geometria (seno, cosseno, tangente), além de fatores aleatórios.

O gerador conta com abertura de UVs automática consoante o shape escolhido, assim como o cálulo das devidas normais para cada vértice, em um arquivo legível pela própria engine do projeto.

Autores:

Luís Almeida: https://github.com/luislcc João Pedro Antunes: https://github.com/MrXester Fernando Lobo: https://github.com/NaNdUSL Diogo Monteiro: https://github.com/monteiro06 
