##Trabalhos de Computação Paralela 2015/2
###Universidade Federal e São Carlos - Campos Sorocaba
---

#####Primeiro Trabalho
---
Trabalho em pthread para resolver o problema de Quebra-senhas. 
Em resumo criamos um algoritmo que cria threads com a ideia do produtor-consumidor.
Dentre um total de threads, escolhemos algumas threads para que criem possíveis senhas e o restante das threads irão testar e validar se é a senha gerada. O programa recebe como entrada a senha que deve ser quebrada, sendo ela em HASH; e o número de threads.

Nesse trabalho há também a versão serializada, utilizamos ela apenas para comparar o desempenho da paralelizada.

#####Segundo Trabalho
---
Trabalhamos com OpenMP, uma forma muito mais abstraída de como criar threads.
Em resumo deste segundo trabalho foi que utilizamos a estratégia de dividir o trabalho entre as threads, sendo que todas as threads criariam senhas e testariam logo em seguida. Sendo assim, cada thread ficaria responsável por uma faixa de senha; por exemplo, thread 1 testaria de 0 a 250 senhas, thread 2 de 251 a 500 ... e assim sucessivamente.

#####Terceiro trabalho
---
Mesmo tema do primeiro trabalho e segundo trablaho, utilizando a mesma estratégia do segundo trabalho, trabalhamos com a MPI (troca de mensagens entre processos).

#####Trabalho Final
---
O tema deste trabalho foi o cálculo da posição das partículas (efeito de partículas), feito em pthread, utilizamos uma thread central para renderizar os objetos e o restante das threads para realizar o cálculo das novas posições de cada partícula. A estratégia escolhida foi divisão do trabalho.


