 #include <iostream>
using namespace std;

class AVL; 
class Nodo {
public: 
    Nodo(const int dat, Nodo* pad = NULL, Nodo* izq = NULL, Nodo* der = NULL) :
           dato(dat), padre(pad), izquierdo(izq), derecho(der), FE(0) {} 
    int dato;
    int FE;
    Nodo* izquierdo;
    Nodo* derecho;
    Nodo* padre;
    friend class AVL;
};

class AVL {
private:
       // enum es un tipo de datos que permite crear una variable   
    enum { Izq, Der };
    // Punteros de la lista, para cabeza y nodo actual:
    Nodo* raiz;
    Nodo* actual;
    int contador;
    int altura;

public:
    // Constructor y destructor básicos:
    AVL() : raiz(NULL), actual(NULL) {}
    ~AVL() { Podar(raiz); }
     
    void Insertar(const int dat);
   
    void Borrar(const int dat); 
    bool Buscar(const int dat); 
    bool Vacio(Nodo* r) { return r == NULL; }
    // Comprobar si es un nodo hoja:
    bool EsHoja(Nodo* r) { return !r->derecho && !r->izquierdo; }
    // Contar número de nodos:
    const int Contar_numeroNodos();
    const int Altura_Arbol();
    // Calcular altura de un dato:
    int Altura(const int dat);
    // Devolver referencia al dato del nodo actual:
    int& ValorActual() { return actual->dato; }
    // Moverse al nodo raiz:
    void Raiz() { actual = raiz; }
    // Aplicar una función a cada elemento del árbol:
    void InOrden(void (*func)(int&, int), Nodo* nodo = NULL, bool r = true);
    void PreOrden(void (*func)(int&, int), Nodo* nodo = NULL, bool r = true);
    void PostOrden(void (*func)(int&, int), Nodo* nodo = NULL, bool r = true);
private: 
    void Equilibrar_Arbol(Nodo* nodo, int, bool);
    void RSI(Nodo* nodo);
    void RSD(Nodo* nodo);
    void RDI(Nodo* nodo);
    void RDD(Nodo* nodo); 
    void Podar(Nodo*&);
    void auxContador_Nodos(Nodo*);
    void auxAltura(Nodo*, int);
};

//   borrar todos los nodos a partir de uno 
void AVL::Podar(Nodo*& nodo)
{
       // Algoritmo recursivo, recorrido en postorden
    if (nodo) {
        Podar(nodo->izquierdo); // Podar izquierdo
        Podar(nodo->derecho);   // Podar derecho
        delete nodo;            // Eliminar nodo
        nodo = NULL;
    }
}

// Insertar un dato en el árbol AVL
void AVL::Insertar(const int dat)
{
    Nodo* padre = NULL; 
   // cout << "El valor : " << dat << endl;
    actual = raiz;
    // Buscar el dato en el árbol, manteniendo un puntero al nodo padre
    while (!Vacio(actual) && dat != actual->dato) {
        padre = actual;
        if (dat > actual->dato) actual = actual->derecho;
        else if (dat < actual->dato) actual = actual->izquierdo;
    } 
    // Si se ha encontrado el elemento, regresar sin insertar
    if (!Vacio(actual)) return;
    // Si padre es NULL, entonces el árbol estaba vacío, el nuevo nodo será el nodo raiz 
    if (Vacio(padre)) raiz = new Nodo(dat);
    // Si el dato es menor que el que contiene el nodo padre, lo insertamos en la rama izquierda 
    else if (dat < padre->dato) {
        padre->izquierdo = new Nodo(dat, padre);
        Equilibrar_Arbol(padre, Izq, true);
    }
    // Si el dato es mayor que el que contiene el nodo padre, lo insertamos en la rama derecha 
    else if (dat > padre->dato) {
        padre->derecho = new Nodo(dat, padre);
        Equilibrar_Arbol(padre, Der, true);
    }
}

// Equilibrar árbol AVL partiendo del nodo nuevo
void AVL::Equilibrar_Arbol(Nodo* nodo, int rama, bool nuevo)
{
    bool salir = false; 
    // Recorrer camino inverso actualizando valores de FE:
    while (nodo && !salir) {
        if (nuevo)
            if (rama == Izq) nodo->FE--; // Depende de si añadimos ...
            else                  nodo->FE++;
        else
            if (rama == Izq) nodo->FE++; // ... o borramos
            else                  nodo->FE--;
        if (nodo->FE == 0) salir = true; // La altura de las rama que
                                        // empieza en nodo no ha variado,
                                        // salir de Equilibrar
        else if (nodo->FE == -2) { // Rotar a derechas y salir:
            if (nodo->izquierdo->FE == 1) RDD(nodo); // Rotación doble
            else RSD(nodo);                         // Rotación simple
            salir = true;
        }
        else if (nodo->FE == 2) {  // Rotar a izquierdas y salir:
            if (nodo->derecho->FE == -1) RDI(nodo); // Rotación doble
            else RSI(nodo);                        // Rotación simple
            salir = true;
        }
        if (nodo->padre)
            if (nodo->padre->derecho == nodo) rama = Der; else rama = Izq;
        nodo = nodo->padre; // Calcular FE, siguiente nodo del camino.
    }
}

// Rotación doble a derechas
void AVL::RDD(Nodo* nodo)
{
    cout << "RDD" << endl;
    Nodo* Padre = nodo->padre;
    Nodo* P = nodo;
    Nodo* Q = P->izquierdo;
    Nodo* R = Q->derecho;
    Nodo* B = R->izquierdo;
    Nodo* C = R->derecho;

    if (Padre)
        if (Padre->derecho == nodo) Padre->derecho = R;
        else Padre->izquierdo = R;
    else raiz = R;

    // Reconstruir árbol:
    Q->derecho = B;
    P->izquierdo = C;
    R->izquierdo = Q;
    R->derecho = P;

    // Reasignar padres:
    R->padre = Padre;
    P->padre = Q->padre = R;
    if (B) B->padre = Q;
    if (C) C->padre = P;

    // Ajustar valores de FE:
    switch (R->FE) {
    case -1: Q->FE = 0; P->FE = 1; break;
    case 0:  Q->FE = 0; P->FE = 0; break;
    case 1:  Q->FE = -1; P->FE = 0; break;
    }
    R->FE = 0;
}

// Rotación doble a izquierdas
void AVL::RDI(Nodo* nodo)
{
    cout << "RDI" << endl;
    Nodo* Padre = nodo->padre;
    Nodo* P = nodo;
    Nodo* Q = P->derecho;
    Nodo* R = Q->izquierdo;
    Nodo* B = R->izquierdo;
    Nodo* C = R->derecho;

    if (Padre)
        if (Padre->derecho == nodo) Padre->derecho = R;
        else Padre->izquierdo = R;
    else raiz = R;

    // Reconstruir árbol:
    P->derecho = B;
    Q->izquierdo = C;
    R->izquierdo = P;
    R->derecho = Q;

    // Reasignar padres:
    R->padre = Padre;
    P->padre = Q->padre = R;
    if (B) B->padre = P;
    if (C) C->padre = Q;

    // Ajustar valores de FE:
    switch (R->FE) {
    case -1: P->FE = 0; Q->FE = 1; break;
    case 0:  P->FE = 0; Q->FE = 0; break;
    case 1:  P->FE = -1; Q->FE = 0; break;
    }
    R->FE = 0;
}

// Rotación simple a derechas
void AVL::RSD(Nodo* nodo)
{
    cout << "RSD" << endl;
    Nodo* Padre = nodo->padre;
    Nodo* P = nodo;
    Nodo* Q = P->izquierdo;
    Nodo* B = Q->derecho;

    if (Padre)
        if (Padre->derecho == P) Padre->derecho = Q;
        else Padre->izquierdo = Q;
    else raiz = Q;

    // Reconstruir árbol:
    P->izquierdo = B;
    Q->derecho = P;

    // Reasignar padres:
    P->padre = Q;
    if (B) B->padre = P;
    Q->padre = Padre;

    // Ajustar valores de FE:
    P->FE = 0;
    Q->FE = 0;
}

// Rotación simple a izquierdas
void AVL::RSI(Nodo* nodo)
{
    cout << "RSI" << endl;
    Nodo* Padre = nodo->padre;
    Nodo* P = nodo;
    Nodo* Q = P->derecho;
    Nodo* B = Q->izquierdo;

    if (Padre)
        if (Padre->derecho == P) Padre->derecho = Q;
        else Padre->izquierdo = Q;
    else raiz = Q;

    // Reconstruir árbol:
    P->derecho = B;
    Q->izquierdo = P;

    // Reasignar padres:
    P->padre = Q;
    if (B) B->padre = P;
    Q->padre = Padre;

    // Ajustar valores de FE:
    P->FE = 0;
    Q->FE = 0;
}

// Eliminar un elemento de un árbol AVL
void AVL::Borrar(const int dat)
{
    Nodo* padre = NULL;
    Nodo* nodo;
    int auxiliar;

    actual = raiz;
    // Mientras sea posible que el valor esté en el árbol
    while (!Vacio(actual)) {
        if (dat == actual->dato) { // Si el valor está en el nodo actual
            if (EsHoja(actual)) { // Y si además es un nodo hoja: lo borramos
                if (padre) // Si tiene padre (no es el nodo raiz)
                   // Anulamos el puntero que le hace referencia
                    if (padre->derecho == actual) padre->derecho = NULL;
                    else if (padre->izquierdo == actual) padre->izquierdo = NULL;
                delete actual; // Borrar el nodo
                actual = NULL;
                // El nodo padre del actual puede ser ahora un nodo hoja, por lo tanto su
                // FE es cero, pero debemos seguir el camino a partir de su padre, si existe.
                if ((padre->derecho == actual && padre->FE == 1) ||
                    (padre->izquierdo == actual && padre->FE == -1)) {
                    padre->FE = 0;
                    actual = padre;
                    padre = actual->padre;
                }
                if (padre)
                    if (padre->derecho == actual) Equilibrar_Arbol(padre, Der, false);
                    else                         Equilibrar_Arbol(padre, Izq, false);
                return;
            }
            else { // Si el valor está en el nodo actual, pero no es hoja
               // Buscar nodo
                padre = actual;
                // Buscar nodo más izquierdo de rama derecha
                if (actual->derecho) {
                    nodo = actual->derecho;
                    while (nodo->izquierdo) {
                        padre = nodo;
                        nodo = nodo->izquierdo;
                    }
                }
                // O buscar nodo más derecho de rama izquierda
                else {
                    nodo = actual->izquierdo;
                    while (nodo->derecho) {
                        padre = nodo;
                        nodo = nodo->derecho;
                    }
                }
                // Intercambiar valores de no a borrar u nodo encontrado
                // y continuar, cerrando el bucle. El nodo encontrado no tiene
                // por qué ser un nodo hoja, cerrando el bucle nos aseguramos
                // de que sólo se eliminan nodos hoja.
                auxiliar = actual->dato;
                actual->dato = nodo->dato;
                nodo->dato = auxiliar;
                actual = nodo;
            }
        }
        else { // Todavía no hemos encontrado el valor, seguir buscándolo
            padre = actual;
            if (dat > actual->dato) actual = actual->derecho;
            else if (dat < actual->dato) actual = actual->izquierdo;
        }
    }
} 
// Recorrido de árbol en inorden 
void AVL::InOrden(void (*func)(int&, int), Nodo* nodo, bool r)
{
    if (r) nodo = raiz;
    if (nodo->izquierdo) InOrden(func, nodo->izquierdo, false);
    func(nodo->dato, nodo->FE);
    if (nodo->derecho) InOrden(func, nodo->derecho, false);
}

// Recorrido de árbol en preorden 
void AVL::PreOrden(void (*func)(int&, int), Nodo* nodo, bool r)
{
    if (r) nodo = raiz;
    func(nodo->dato, nodo->FE);
    if (nodo->izquierdo) PreOrden(func, nodo->izquierdo, false);
    if (nodo->derecho) PreOrden(func, nodo->derecho, false);
}

// Recorrido de árbol en postorden 
void AVL::PostOrden(void (*func)(int&, int), Nodo* nodo, bool r)
{
    if (r) nodo = raiz;
    if (nodo->izquierdo) PostOrden(func, nodo->izquierdo, false);
    if (nodo->derecho) PostOrden(func, nodo->derecho, false);
    func(nodo->dato, nodo->FE);
}

// Buscar un valor en el árbol
bool AVL::Buscar(const int dat)
{
    actual = raiz;

    // Todavía puede aparecer, ya que quedan nodos por mirar
    while (!Vacio(actual)) {
        if (dat == actual->dato) return true; // dato encontrado
        else if (dat > actual->dato) actual = actual->derecho; // Seguir
        else if (dat < actual->dato) actual = actual->izquierdo;
    }
    return false; // No está en árbol
}

// Calcular la altura del nodo que contiene el dato dat
int AVL::Altura(const int dat)
{
    int altura = 0;
    actual = raiz;  //en dado caso qu encontara un dato 
    while (!Vacio(actual)) {
        if (dat == actual->dato) return altura;  
        else {
            altura++; // Incrementamos la altura, seguimos buscando
            if (dat > actual->dato) actual = actual->derecho;
            else if (dat < actual->dato) actual = actual->izquierdo;
        }
    }
    return -1; // en dado caso de  No estár en árbol
}

// Contar el número de nodos
const int AVL::Contar_numeroNodos()
{
    contador = 0; 
    auxContador_Nodos(raiz);  
    return contador;
}

// Función auxiliar para contar nodos. Función recursiva de recorrido enpreorden
void AVL::auxContador_Nodos(Nodo* nodo)
{
    contador++; 
    // Continuar recorrido
    if (nodo->izquierdo) auxContador_Nodos(nodo->izquierdo);
    if (nodo->derecho)   auxContador_Nodos(nodo->derecho);
}

// Calcular la altura del árbol, que es la altura del nodo de mayor altura.
const int AVL::Altura_Arbol()
{
    altura = 0; 
    auxAltura(raiz, 0);  
    return altura;
}

// Función auxiliar para calcular altura. Función recursiva de recorrido en postorden
void AVL::auxAltura(Nodo* nodo, int a)
{
    // Recorrido postorden
    if (nodo->izquierdo) auxAltura(nodo->izquierdo, a + 1);
    if (nodo->derecho)   auxAltura(nodo->derecho, a + 1);
    // Proceso, si es un nodo hoja, y su altura es mayor que la actual del
    // árbol, actualizamos la altura actual del árbol
    if (EsHoja(nodo) && a > altura) altura = a;
}

// Función de prueba para recorridos del árbol
void Mostrar(int& d, int FE)
{
    cout << d << "(" << FE << "),";
}

int main()
{ 
    AVL ArbolInt; 
    ArbolInt.Insertar(5);
    ArbolInt.Insertar(7);
    ArbolInt.Insertar(9);
    ArbolInt.Insertar(8);
    ArbolInt.Insertar(10);
    
    

    cout << "Altura de arbol " << ArbolInt.Altura_Arbol() << endl; 
  
    cout << "\nEl InOrden es \t(IRD) :\t";
    ArbolInt.InOrden(Mostrar);
    cout << endl;
    cout << "\nEl PreOrden es \t(RID) :\t";
    ArbolInt.PreOrden(Mostrar);
    cout << endl;
    cout << "\nEl PostOrden es \t(IDR) :\t";
    ArbolInt.PostOrden(Mostrar);
    cout << endl;
 
    cin.get();
    return 0;
}
