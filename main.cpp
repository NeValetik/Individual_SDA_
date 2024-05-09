#include "components/data.hpp"


const int MONEY_HOLD = 13700;
const int MAXIMAL_MONEY_HOLD = MONEY_HOLD * 5;
const float MINIMAL_PENSION_FOR_FULL_STAGE = 2778.5;
const float MINIMAL_PENSION_FOR_FIRST_DISSABILITY = 1965.47;
const float MINIMAL_PENSION_FOR_SECOND_DISSABILITY = 1834.43;
const float MINIMAL_PENSION_FOR_THIRD_DISSABILITY = 1310.31;
const float MINIMAL_PENSION_FOR_NOT_FULL_STAGE = MINIMAL_PENSION_FOR_FULL_STAGE * 0.5;


float calculate_sum(Pension person){
    float sa = 0;
    float years_sum = 0;
    for(int i = 0; i<person.sal.sum.size();i++){
        if(person.sal.sum[i] - MAXIMAL_MONEY_HOLD> 0){
            // std::cout<<"Hello";
            sa += MAXIMAL_MONEY_HOLD*person.sal.time_period[i];
        }else{     
        sa += person.sal.sum[i]*person.sal.time_period[i];
        }
        years_sum += person.sal.time_period[i];
    };
    return sa/(years_sum) ;
} 


float pension_for_stage_one(Pension person,std::vector<std::vector<std::string>> data){
    const int VL = 34; //необходимый страховой стаж на дату получения права на пенсию;
    int RL = 63;
    for (const auto& row:data){
        if ( !row.empty() &&person.work == row[0]){
            RL = std::stoi(row[1]);
            break;
        } 
    }
    float sa = calculate_sum(person);
    float pension = (0.014 * 34 +0.02 *(person.experience_in_years-VL) + 0.02* (person.age - RL))*sa;
    if(pension < MINIMAL_PENSION_FOR_FULL_STAGE) return MINIMAL_PENSION_FOR_FULL_STAGE;
    return pension;
}


float pension_for_stage_two(Pension person){
    float sa = calculate_sum(person);
    float pension = 0.014 * person.experience_in_years*sa;
    if(pension < MINIMAL_PENSION_FOR_NOT_FULL_STAGE) return MINIMAL_PENSION_FOR_NOT_FULL_STAGE;
    return pension;
}


float pension_for_disability(Pension person){
    const int V_MAX = 39; //необходимый страховой стаж на дату получения права на пенсию;
    switch(person.stage_of_dissability){
        case 1:{
            float sa = calculate_sum(person);
            float pension;
            float v_dependent_of_age = person.age-18;
            if(V_MAX>v_dependent_of_age){
                pension = 0.42 * sa + (person.experience_in_years / v_dependent_of_age) * sa * MONEY_HOLD * 0.1;
            }else{
                pension = 0.42 * sa + (person.experience_in_years / V_MAX) * sa * MONEY_HOLD * 0.1;
            }
            if(pension < MINIMAL_PENSION_FOR_FIRST_DISSABILITY) return MINIMAL_PENSION_FOR_FIRST_DISSABILITY;
            return pension;
        }
        case 2:{
            float sa = calculate_sum(person);
            float pension;
            float v_dependent_of_age = person.age-18;
            if(V_MAX>v_dependent_of_age){
                pension = 0.35 * sa + (person.experience_in_years / v_dependent_of_age) * sa * MONEY_HOLD * 0.1;
            }else{
                pension = 0.35 * sa + (person.experience_in_years / V_MAX) * sa * MONEY_HOLD * 0.1;
            }
            if(pension < MINIMAL_PENSION_FOR_SECOND_DISSABILITY) return MINIMAL_PENSION_FOR_SECOND_DISSABILITY;
            return pension;
        }
        case 3:{
            float sa = calculate_sum(person);
            float pension;
            float v_dependent_of_age = person.age-18;
            if(V_MAX>v_dependent_of_age){
                pension = 0.2 * sa + (person.experience_in_years / v_dependent_of_age) * sa * MONEY_HOLD * 0.1;
            }else{
                pension = 0.2 * sa + (person.experience_in_years / V_MAX) * sa * MONEY_HOLD * 0.1;
            }
            if(pension < MINIMAL_PENSION_FOR_THIRD_DISSABILITY) return MINIMAL_PENSION_FOR_THIRD_DISSABILITY;
            return pension;
        }
        default: 
            return 0;
    }
}


float pension_for_survivor_loss(Pension person){
    if(person.age<=23){
        switch(person.children_number){
            case 1:{
                float pension = 0.5 * person.kormilici_pension_sum;
                return pension;
            }
            case 2:{
                float pension = 0.75 * person.kormilici_pension_sum;
                return pension;
            }
            default:{
                if(person.children_number>=3){
                    float pension = person.kormilici_pension_sum;
                    return pension;
                }
            }
        }
    }else{
        std::cout<<"You cannot have the pension you're too mature.";
    }
    return 0;
}


void print_pension(std::vector<Pension> persons,std::vector<std::vector<std::string>> data){
    for(auto pers : persons){
        if (pers.type == pension_type::ACCUMULATED){
            pers.display();
            if(pers.experience_in_years>=34){
                std::cout<<"Expected pension: "<<pension_for_stage_one(pers,data)<<"\n";
            }else if(pers.experience_in_years>15 && pers.experience_in_years<35){
                std::cout<<"Expected pension: "<<pension_for_stage_two(pers)<<"\n";
            }else{
                std::cout<<"The person "<< pers.name <<" has not enough stage for pension.";
            }
        }else if(pers.type == pension_type::DISABILITY){
            pers.display();
            std::cout<<"Expected pension: "<<pension_for_disability(pers)<<"\n";
        }else if(pers.type == pension_type::SURVIVOR_LOSS){
            pers.display();
            std::cout<<"Expected pension: "<<pension_for_survivor_loss(pers)<<"\n";
        }
    }
}


int main(){
    std::vector<Pension> persons;
    std::ifstream file("components/dataset.csv"); // Open the CSV file

    if (!file.is_open()) {
        std::cout << "Error opening file!" << std::endl;
        return 1;
    }

    std::vector<std::vector<std::string>> data;
    std::string line;
    
    while (getline(file, line)) { 
        std::stringstream ss(line);
        std::vector<std::string> row;
        std::string cell;

        while (getline(ss, cell, ',')) { 
            row.push_back(cell);
        }

        data.push_back(row); 
    }

    file.close();

    std::string temp = "";


    while(!(temp.find("e")!= std::string::npos)){
        Pension person; 
        persons.push_back(person);
        std::cout<<"Type 'end'<e> if you want to stop introducing or 'go'<any> to continue introducing persons.\n";
        std::cin>>temp;
        getchar();
    }    
    print_pension(persons,data);
    getchar();
    return 200;
}