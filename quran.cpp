#include <iostream>
#include <fstream>
#include <string>

// عرض القرآن كما هو – المصدر: ملف اسمه ١
void truerealityinfinitedimensionalunlimitedfreedom(const std::string& اسم_الجلالة)
{
    std::ifstream المصحف(u8"١");
    if (!المصحف.is_open())
        return;

    std::string آية;
    while (std::getline(المصحف, آية))
    {
        std::cout << آية << '\n';
    }
}

int main()
{
    std::string اسم_الجلالة = u8"ﷲ";
    truerealityinfinitedimensionalunlimitedfreedom(اسم_الجلالة);
    return 0;
}
