#include <iostream>
#include <sstream>

//Адаптируемый класс
class GasCylinder{
public:
    GasCylinder(double v, double m, double M): Volume(v), Mass(m), Molar(M) {};
    double GetPressure(int T) const{
        return Mass * R * T /(Molar * Volume);
    }
    double AmountOfMatter() const{
        return Mass / Molar;
    }
    std::string ToString() const{
        std::ostringstream strm;
        strm << Volume << " " <<  Mass  << " " <<  Molar;
        return strm.str();
    }
protected:
    double Volume;
    double Mass;
    double Molar;
private:
    inline static const double R = 8.314; //Универсальная газовая постоянная
};

//Требуемый интерфейс
class Balloon{
public:
    virtual ~Balloon() = default;
    virtual double CalculateDp(int T0, int dT) = 0;
    virtual void ModifMass(double dm) = 0;
    virtual std::string GetData() = 0;
};

//Адаптер
class GasCylinderAdapter: public Balloon{
public:
    explicit GasCylinderAdapter(GasCylinder* g): gas_cylinder(g) {
    };
    ~GasCylinderAdapter(){
        delete gas_cylinder;
    }
    double CalculateDp(int T0, int dT){
        return gas_cylinder->GetPressure(T0 + dT) - gas_cylinder->GetPressure(T0);
    }
    void ModifMass(double dm){
        std::string data = gas_cylinder->ToString();
        std::stringstream strm(data);
        double V; strm >> V;
        double m; strm >> m;
        double M; strm >> M;
        if (m + dm < 0){
            throw std::out_of_range("Incorrect mass change: mass becomes negative");
        }
        GasCylinder new_cylinder(V, m+dm, M);
        *gas_cylinder = new_cylinder;
    }
    std::string GetData(){
        std::string data = gas_cylinder->ToString();
        std::stringstream strm(data);
        double V; strm >> V;
        double m; strm >> m;
        double M; strm >> M;

        std::stringstream nstrm;
        nstrm << "Volume:" << V << "   Mass:" << m << "   Molar:" << M;
        return nstrm.str();
    }
private:
    GasCylinder* gas_cylinder;
};

int main() {
    auto* p = new GasCylinderAdapter(new GasCylinder(1000, 1000, 1)); //Водород
    std::cout << "Change in pressure with increasing temperature:" << p->CalculateDp(300, 100) << std::endl;
    std::cout << "Change in pressure with decreasing temperature:" << p->CalculateDp(300, -100) << std::endl;
    std::cout << "Before mass changing:  " << p->GetData() << std::endl;
    try{
        p->ModifMass(-1500);
    }
    catch(std::out_of_range e){
        std::cout << "Trying set mass negative - ";
        std::cout << e.what() << std::endl;
    }
    p->ModifMass(500);
    std::cout << "After mass changing:  " << p->GetData() << std::endl;
    delete p;
    return 0;
}
