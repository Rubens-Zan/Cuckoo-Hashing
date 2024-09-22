# Objetivo
Neste exercício iremos implementar a inclusão e exclusão de valores em tabela hash de endreçamento aberto.


## Tabelas Hash:
- O trabalho terá duas tabelas hash de tamanho igual T1 e T2. 
- Assuma tamanho m = 11. 
- A função hash tem a assinatura h(k), onde k é a chave inteira. 
- A primeira tabela T1 possui função hash "h1(k) = k mod m".
-  A segunda tabela T2 possui função hash "h2(k) = ⌊m * (k * 0.9 − ⌊k * 0.9⌋)⌋", onde "⌊ ⌋" significa o arredondamento para inteiro imediatamente menor (i.e., chão) de um valor real. Trataremos colisão movendo chaves entre T1 e T2 ao invés de usar sondagem (e.g., linear ou quadrática). Esse exercício simula o algoritmo de Cuckoo Hash, mas sem nos preocuparmos com rehashing.

## Algoritmo de busca:
- Se a posição da chave k calculada por h1(k) estiver vazia, então retorna nulo e a chave não existe (devido nosso tratamento de colisão na inclusão e exclusão).
- Se a chave k existe em T1, retorna a posição dela em T1 com h1(k).
    Senão, a chave k existe em T2, retorna a posição dela em T2 com h2(k).

## Algoritmo de inclusão:

    A inclusão deve ser feita sempre em T1: Se a posição calculada por h1(k) estiver vazia ou marcada como "excluída", então insira a chave k.
    Senão, temos colisão em T1 e devemos realizar duas operações:
        considere a chave atual como ki e a nova chave a ser inserida como kj e considere a colisão quando h1(ki) == h1(kj)
        copiar ki que está em h1(ki) de T1 para T2. O objetivo é permitir inclusão da nova chave kj em T1 sem perder ki na colisão. A chave ki deverá ser incluida em T2 usando h2(ki).
        incluir a nova chave kj na posição h1(kj) de T1.
    A estrutura deverá ignorar chaves duplicadas neste caso simplesmente mantendo a primeira chave inserida.

- Considere como exemplo a inclusão das seguintes chaves em sequencia: k = {10, 22, 4, 15, 59}. 
- A imagem abaixo mostra as tabelas T1 e T2 após as inclusões e um log das operações. 
- As chaves 10 e 22 não tiveram colisão. 
- As chaves 4, 15 e 59 tiveram colisão. Em T1 somente deve ficar a chave 59, pois foi a última a ser inserida. - As chaves 4 e 15 foram movidas para T2 seguindo o algoritmo e estão escritas em vermelho em T1 somente para mostrar que foram retiradas de T1. Como não nos preocuparemos com rehashing, os casos de teste não terão colisão em T2.

![alt text](image.png)

## Algoritmo de exclusão:

    Podemos excluir qualquer chave de qualquer tabela. Considere que a chave a ser excluída ki existe em T1 ou T2.
    Se a chave ki estiver na posição calculada por h2(ki) em T2, então apenas exclua a chave ki de T2.
    Senão, a chave está na posição calculada por h1(ki) em T1. Neste caso precisamos ter cuidado, pois podem existir chaves inseridas em T2 que dependem de uma busca em T1 e se encontrarem um valor nul a busca retorna "chave inexistente". Então exclua ki de h1(ki) em T1 e marque a posição em T1 como "excluída" para que uma busca não encontre posição nula e devolva chave não encontrada. Não esqueça de alterar o algoritmo de busca para considerar o estado de "excluída".

Considere como exemplo a exclusão das seguintes chaves em sequencia: k = {15, 22, 59}. As posições das chaves 22 e 59 em T1 foram marcadas como excluida pois não sabemos se existem chaves em T2 que dependem dessa posição para a busca.

![alt text](image-1.png)


## Entrada:

A entrada deve ser feita pela entrada padrão (stdin). O arquivo é formado por uma sequência de linhas, onde cada linha representa uma operação com uma chave inteira. A ordem das operações é aleatória. Cada operação pode ser de inclusão (i) ou remoção (r). O primeiro campo é o tipo da operação e o segundo campo é a chave. Por exemplo, "i 10" significa a inclusão da chave 10 na tabela hash.
Saída:

A saída deve ser feita pela saída padrão (stdout). A escrita na saída pode acontecer ao final das operações de entrada. O arquivo será composto por uma sequência de linhas. Uma linha para cada valor lido no formato (chave, tabela, posição). Portanto, cada linha tem 3 campos separados por vírgula. O primeiro campo é a chave armazenada em uma das tabelas hash. O segundo campo é o nome da tabela hash de armazenamento: T1 ou T2. O terceiro campo é a posição da chave em T1 ou T2. A saida será ordenada pelo primeiro campo, seguido pelo segundo e terceiro campos.

Exemplo de arquivos com uma entrada e uma saída válida:


Entrada	
i 10
i 22
i 4
i 15
i 59
r 15
r 22
r 59
 
Saída
4,T2,6
10,T1,10

## Requisitos mínimos:

O trabalho deve ser feito em linguagem de programação C/C++ de forma que possa ser compilado e executado nas servidoras de computação do Departamento de Informática. Além disso, o executável não deve ter nenhuma opção de linha comando. O nome do executável deve ser: myht. 
O que deve ser entregue:

Além dos arquivos fonte, deve acompanhar um makefile e um relatório com no máximo 2 páginas contendo a documentação sintetizada do sistema implementado. Qualquer particularidade deve estar descrita neste texto. Relatórios sem os nomes dos alunos não serão corrigidos.

Para compilar será usado o comando make (sem nenhum parâmetro), portanto preparem o Makefile para fazer isso.

Para testar será executado no terminal Linux um script com os comandos exatamente como exemplificado abaixo. Os símbolos "<" e ">" representam a entrada padrão (stdin) e a saída padrão (stdout).

$ ./myht < teste.in > teste.out
$ diff teste.sol teste.out

Onde teste.in é o arquivo de entrada do teste e teste.sol é o esperado como saída.
Caso o teste seja positivo (não imprime nada) será analisado o código fonte e o relatório.
Os fontes também devem ser comentados para uma possível análise.

## O que deve ser entregue:
Além dos arquivos fonte, deve acompanhar um makefile e um relatório com no máximo 2 páginas contendo a documentação sintetizada do sistema implementado. Qualquer particularidade deve estar descrita neste texto. Relatórios sem os nomes dos alunos não serão corrigidos.

Para compilar será usado o comando make (sem nenhum parâmetro), portanto preparem o Makefile para fazer isso.

Para testar será executado no terminal Linux um script com os comandos exatamente como exemplificado abaixo. Os símbolos "<" e ">" representam a entrada padrão (stdin) e a saída padrão (stdout).

$ ./myht < teste.in > teste.out
$ diff teste.sol teste.out

Onde teste.in é o arquivo de entrada do teste e teste.sol é o esperado como saída.
Caso o teste seja positivo (não imprime nada) será analisado o código fonte e o relatório.
Os fontes também devem ser comentados para uma possível análise.