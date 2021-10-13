#include "disjoint_set.h"
#include <iostream>

#define ROOT -1 // объявили макрос (считайте константу) равный минус единице - чтобы не было "волшебной переменной", а была именованная константа "корень дерева"


DisjointSet::DisjointSet(int size)
{
    this->parents = std::vector<int>(size);
    this->ranks = std::vector<int>(size);
    this->sizes = std::vector<int>(size);

    for (size_t i = 0; i < size; i++)
    {
        this->parents[i] = ROOT;
        this->ranks[i] = 1;
        this->sizes[i] = 1;
    }
    // TODO - заполните вектора так чтобы на этапе конструирования эта система непересекающихся множеств состояла из:
    // size элементов, каждый из которых состоит в своем множестве (а значит ссылка на родителя у него - на корень, т.е. на ROOT, а какой тогда ранк и размер у множества каждого элемента?)
    // TODO заполнить parents
    // TODO заполнить ranks
    // TODO заполнить sizes
}

int	DisjointSet::get_set(int element)
{
    // TODO по номеру элемента нужно переходя по ссылкам на родителя дойти до самого верхнего элемента,
    // номер этого корневого элемента - номер множества на данный момент (кто вверху тот и главный, множество названо в его честь)
    int first_of_their_kind = element;
    while(this->parents[first_of_their_kind] != ROOT)
    {
        first_of_their_kind = this->parents[first_of_their_kind];
    }
    if(first_of_their_kind != element) this->parents[element] = first_of_their_kind;

    return first_of_their_kind;
}

int DisjointSet::count_differents() const
{
    // TODO посчитать сколько разных множеств (подсказка: в каждом множестве ровно один корень, а корень - это тот у кого родитель = ROOT)
    int count = 0;
    for (size_t i = 0; i < this->parents.size(); i++) 
    {
        if (this->parents[i] == ROOT) count++;
    }
    return count;
}

int DisjointSet::get_set_size(int element)
{
    // TODO сообщить сколько элементов в множестве, которому принадлежит данный элемент (да, это очень просто)
    return this->sizes[get_set(element)];
}

int	DisjointSet::union_sets(int element0, int element1)
{
    int parent0 = get_set(element0);
    int parent1 = get_set(element1);
    if (this->ranks[parent0] == this->ranks[parent1])
    {
        this->parents[parent1] = parent0;
        this->ranks[parent0]++;
        this->sizes[parent0] += this->sizes[parent1];
        return parent0;
    }
    else if (this->ranks[parent0] > this->ranks[parent1])
    {
        this->parents[parent1] = parent0;
        this->sizes[parent0] += this->sizes[parent1];
        return parent0;
    }
    else
    {
        this->parents[parent0] = parent1;
        this->sizes[parent1] += this->sizes[parent0];
        return parent1;
    }
    // TODO узнать корневые элементы этих двух элементов и посмотрев на них - решить,
    // кого из них подвесить к другому (тем самым объединить два множества)
    // при этом стоит подвешивать менее высокое дерево к более высокому (т.е. учитывая ранк),
    // а так же важно не забыть после переподвешивания у корневого элемента обновить ранк и размер множества

    return ROOT; // вернуть номер объединенного множества
}
