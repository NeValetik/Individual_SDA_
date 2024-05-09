#include <vector>
#include <iostream>
#include <string>
#include <bits/stdc++.h>
#include <fstream>
#include <sstream>


enum class pension_type {ACCUMULATED, DISABILITY,SURVIVOR_LOSS, UNDEFINED};
enum class payment_type {CONSTANT, DIFFERENT,UNDEFINED};


bool is_digits(std::string& str){
    for (char ch : str) {
        int v = ch; // ASCII Val converted
        if (!(ch >= 46 && ch <= 57 )) {
            return false;
        }
    }
    return true;
}


bool is_digits_int(std::string& str){
    for (char ch : str) {
        int v = ch; // ASCII Val converted
        if (!(ch >= 48 && ch <= 57 )) {
            return false;
        }
    }
    return true;
}


class Pension{
    public:
    struct experience{
        int years;
        int months;
        int days;
    };

    struct salary{
        std::vector<float> sum;
        std::vector<float> time_period;
    };

    std::string name;
    std::string gender;
    std::string work;
    struct experience exp;
    struct experience birth_date;
    pension_type type = pension_type::UNDEFINED;
    payment_type pay_type = payment_type::UNDEFINED;
    int stage_of_dissability = 0;//1/2/3
    struct salary sal;
    float experience_in_years;
    float age; 
    std::vector<float> kormilici_pensions;//кормильцы
    int children_number;
    float kormilici_pension_sum;
    int number_of_kormilici;

    Pension(){
        std::cout<<"Name:\n";
        std::getline(std::cin,this->name);

        std::cout<<"Sex(F/m):\n";
        std::getline(std::cin,this->gender);
        while(this->gender!="f" && this->gender!="m" && this->gender!="F" && this->gender!="M"){
            std::cout<<"You introduced the unregistered value. Introduce again(F/m):\n";
            std::getline(std::cin,this->gender);
        }

        std::cout<<"Type of Pension(Accumulated<acc>/Disability<dis>/Survivor loss<surv>):\n";
        while(this->type == pension_type::UNDEFINED){//Here Pension Definition
            std::string temp;
            std::cin>> temp;
            this->type = pension_string_to_type(temp);
            if (this->type == pension_type::UNDEFINED){
                std::cout<<"Wrong type of pension, introduce again:\n";    
            }
        } 
        
        
        if (this->type == pension_type::ACCUMULATED){//Here Pension
            std::cout<<"Proficiency:\n";
            std::cin>>this->work;
            std::cout<<"Birth date (dd mm yyyy):\n";
            std::cin>>this->birth_date.days>>this->birth_date.months>>this->birth_date.years;
            experience_error_handling(&this->birth_date.years,&this->birth_date.months,&this->birth_date.days,true);
            this->age = float(2024 - this->birth_date.years) + float(this->birth_date.months)/12 + float(this->birth_date.days)/365;
            
            std::cout<<"Stage (yy mm dd) (introduce 0 to fill up):\n";
            std::cin>>this->exp.years>>this->exp.months>>this->exp.days;
            experience_error_handling(&this->exp.years,&this->exp.months,&this->exp.days,false);
            this->experience_in_years = float(this->exp.years) + float(this->exp.months)/12 + float(this->exp.days)/365;
            while (!check_for_birth_day_expirience_correctness()){
                std::cout<<"The stage and birthday are not corresponding. You can set stage to (0 0 0) to skip.\n";
                std::cout<<"Stage (yy mm dd) (introduce 0 to fill up):\n";
                std::cin>>this->exp.years>>this->exp.months>>this->exp.days;
                experience_error_handling(&this->exp.years,&this->exp.months,&this->exp.days,false);
                this->experience_in_years = float(this->exp.years) + float(this->exp.months)/12 + float(this->exp.days)/365;
            }
            if(this->experience_in_years != 0){//Here Payment
                std::cout<<"Was your salary constant or different?(Constant/Different):\n";
                this->pay_type = payment_type::UNDEFINED;
                while(this->pay_type == payment_type::UNDEFINED){
                    std::string temp;
                    std::cin>> temp;
                    this->pay_type = payment_string_to_type(temp);
                    if (this->pay_type == payment_type::UNDEFINED){
                        std::cout<<"Wrong type of payment, introduce again:\n";    
                    }
                }
                if (this->pay_type == payment_type::CONSTANT){
                    std::cout<<"Give your salary size (in float):\n";
                    std::string temp;
                    std::cin>>temp;
                    while(!is_digits(temp)){
                        std::cout<<"The introduced value is not a number, introduce again:\n";
                        std::cin>>temp;
                    }
                    float temp_sum = std::stof(temp);
                    this->sal.sum.push_back(temp_sum);
                    this->sal.time_period.push_back(this->experience_in_years);
                }else if (this->pay_type == payment_type::DIFFERENT){
                    std::cout<<"Give your salary size(in float) and time period (in years)| Type 'e' to stop introducing data and fill up the remaining stage with time:\n";
                    std::string temp;
                    float counter = 0;
                    
                    while(temp!="e" && is_digits(temp) && this->experience_in_years - counter> 1e-4){
                        std::cout<<"Sum or end:\n";
                        std::cin>>temp;
                        while(!is_digits(temp) && temp != "e"){
                            std::cout<<"The introduced value is not a number, introduce again:\n";
                            std::cin>>temp;
                        }
                        if(temp!="e" && is_digits(temp)){
                            float temp_sum = std::stof(temp);
                            this->sal.sum.push_back(temp_sum);
                            std::cout<<"Time:\n";
                            float temp_time;
                            std::cin>>temp_time;
                            counter += temp_time;

                            
                            if(time_period_is_satisfying(counter,this->experience_in_years)){
                                this->sal.time_period.push_back(temp_time);
                            }else{
                                while(!time_period_is_satisfying(counter,this->experience_in_years)){
                                    counter -= temp_time;
                                    std::cout<<"You don't have this amount of time\n"<<"The time you have left is: "<<this->experience_in_years-counter<<"\nIntroduce the time again: ";
                                    std::cin>>temp_time;
                                    counter += temp_time;
                                }
                                this->sal.time_period.push_back(temp_time);
                            }
                            if(this->experience_in_years - counter< 1e-4){
                                std::cout<<"\nYou don't have any time left\n"<<std::endl;
                            }
                        }else if(temp == "e"){
                            this->sal.time_period.push_back(this->experience_in_years - counter);
                            this->sal.sum.push_back(this->sal.sum.back());
                        }
                    }
                }
            }else{
                this->sal.sum.push_back(0);
                this->sal.time_period.push_back(0);
            }   
        }
        
        
        else if (this->type == pension_type::DISABILITY){//Here Pension
            std::cout<<"Introduce the dissability stage(3/2/1):\n";
            while(stage_of_dissability != 1 && stage_of_dissability != 2 && stage_of_dissability != 3){
                std::cin>>this->stage_of_dissability;
                if(stage_of_dissability != 1 && stage_of_dissability != 2 && stage_of_dissability != 3)
                std::cout<<"Introduce again the dissability stage(3/2/1):\n";
            }
            std::cout<<"Birth date (dd mm yyyy):\n";
            std::cin>>this->birth_date.days>>this->birth_date.months>>this->birth_date.years;
            experience_error_handling(&this->birth_date.years,&this->birth_date.months,&this->birth_date.days,true);
            this->age = float(2024 - this->birth_date.years) + float(this->birth_date.months)/12 + float(this->birth_date.days)/365;
            
            std::cout<<"Stage (yy mm dd) (introduce 0 to fill up):\n";
            std::cin>>this->exp.years>>this->exp.months>>this->exp.days;
            experience_error_handling(&this->exp.years,&this->exp.months,&this->exp.days,false);
            this->experience_in_years = float(this->exp.years) + float(this->exp.months)/12 + float(this->exp.days)/365;
            while (!check_for_birth_day_expirience_correctness()){
                std::cout<<"The stage and birthday are not corresponding. You can set stage to (0 0 0) to skip.\n";
                std::cout<<"Stage (yy mm dd) (introduce 0 to fill up):\n";
                std::cin>>this->exp.years>>this->exp.months>>this->exp.days;
                experience_error_handling(&this->exp.years,&this->exp.months,&this->exp.days,false);
                this->experience_in_years = float(this->exp.years) + float(this->exp.months)/12 + float(this->exp.days)/365;
            }
            if(this->experience_in_years != 0){//Here Payment
                std::cout<<"Was your salary constant or different?(Constant/Different):\n";
                while(this->pay_type == payment_type::UNDEFINED){
                    std::string temp;
                    std::cin>> temp;
                    this->pay_type = payment_string_to_type(temp);
                    if (this->pay_type == payment_type::UNDEFINED){
                        std::cout<<"Wrong type of payment, introduce again:\n";    
                    }
                }
                if (this->pay_type == payment_type::CONSTANT){
                    std::cout<<"Give your salary size (in float):\n";
                    std::string temp;
                    std::cin>>temp;
                    while(!is_digits(temp)){
                        std::cout<<"The introduced value is not a number, introduce again:\n";
                        std::cin>>temp;
                    }
                    float temp_sum = std::stof(temp);
                    this->sal.sum.push_back(temp_sum);
                    this->sal.time_period.push_back(this->experience_in_years);
                }else if (this->pay_type == payment_type::DIFFERENT){
                    std::cout<<"Give your salary size(in float) and time period (in years)| Type 'e' to stop introducing data and fill up the remaining stage with time:\n";
                    std::string temp;
                    float counter = 0;

                    while(temp!="e" && is_digits(temp) && this->experience_in_years - counter> 1e-4){
                        std::cout<<"Sum or end:\n";
                        std::cin>>temp;
                        while(!is_digits(temp) && temp != "e"){
                            std::cout<<"The introduced value is not a number, introduce again:\n";
                            std::cin>>temp;
                        }
                        if(temp!="e" && is_digits(temp)){
                            float temp_sum = std::stof(temp);
                            this->sal.sum.push_back(temp_sum);
                            std::cout<<"Time:\n";
                            float temp_time;
                            std::cin>>temp_time;
                            counter += temp_time;

                            
                            if(time_period_is_satisfying(counter,this->experience_in_years)){
                                this->sal.time_period.push_back(temp_time);
                            }else{
                                while(!time_period_is_satisfying(counter,this->experience_in_years)){
                                    counter -= temp_time;
                                    std::cout<<"You don't have this amount of time\n"<<"The time you have left is: "<<this->experience_in_years-counter<<"\nIntroduce the time again: ";
                                    std::cin>>temp_time;
                                    counter += temp_time;
                                }
                                this->sal.time_period.push_back(temp_time);
                            }
                            if(this->experience_in_years - counter< 1e-4){
                                std::cout<<"\nYou don't have any time left\n"<<std::endl;
                            }
                        }else if(temp == "e"){
                            this->sal.time_period.push_back(this->experience_in_years - counter);
                            if(this->sal.sum.size() != 0){
                                this->sal.sum.push_back(this->sal.sum.back());
                            }else{
                                this->sal.sum.push_back(0);
                            }
                        }
                    }
                }
            }else{
                this->sal.sum.push_back(0);
                this->sal.time_period.push_back(0);
            }   
        }
        
        
        else if (this->type == pension_type::SURVIVOR_LOSS){//Here Pension
            std::cout<<"Introduce the number of children(int)(1/2/3+):\n";
            std::string kids;
            std::cin>>kids;
            while(!is_digits_int(kids) || kids == "0"){
                std::cout<<"You introduce the wrong value, introduce again:\n";
                std::cin>>kids;
            }
            this->children_number = std::stoi(kids);
            
            std::cout<<"Birth date (dd mm yyyy):\n";
            std::cin>>this->birth_date.days>>this->birth_date.months>>this->birth_date.years;
            experience_error_handling(&this->birth_date.years,&this->birth_date.months,&this->birth_date.days,true);
            this->age = float(2024 - this->birth_date.years) + float(this->birth_date.months)/12 + float(this->birth_date.days)/365;
            
            float counter = 0;
            std::string temp;
            while(temp!= "e"){
                std::cout<<"Introduce the size of pension of the deseiced survivor.Type 'e' to stop introducing :\n";
                std::cin>>temp;
                while(!is_digits(temp) && temp!="e"){
                    std::cout<<"The introduced value is not a number, introduce again:\n";
                    std::cin>>temp;
                }
                if(temp!="e"){    
                    float temp_sum = std::stof(temp);
                    this->kormilici_pensions.push_back(temp_sum);
                    counter += temp_sum;
                }
            }
            this->kormilici_pension_sum = counter;
            this->number_of_kormilici = kormilici_pensions.size();
        }      
    }


    void display(){//Displaying the Person info
        switch(this->type){
            case pension_type::ACCUMULATED:
                std::cout<<this->name<<" Sex: "<<this->gender<<" Work: "<<this->work<<" Birth date: {"<<"D:"<<this->birth_date.days<<" M:"<<this->birth_date.months<<" Y:"<<this->birth_date.years<<"} "<<" Stage: {Y:"<<this->exp.years<<" M:"<<this->exp.months<<" D:"<<this->exp.days<<"} "<<"Salary size: {" << vector_sal_as_string()<<"}"<<std::endl;
                break;
            case pension_type::DISABILITY:
                std::cout<<this->name<<" Sex: "<<this->gender<<" Stage_of_dissability: "<<this->stage_of_dissability<<" Birth date: {"<<"D:"<<this->birth_date.days<<" M:"<<this->birth_date.months<<" Y:"<<this->birth_date.years<<"} "<<" Stage: {Y:"<<this->exp.years<<" M:"<<this->exp.months<<" D:"<<this->exp.days<<"} "<<"Salary size: {" << vector_sal_as_string()<<"}"<<std::endl;
                break;
            case pension_type::SURVIVOR_LOSS:
                std::cout<<this->name<<" Sex: "<<this->gender<<" Children number: "<< this->children_number <<" Pensions: {"<<vector_pension_survivor_as_string()<<"}"<<std::endl;
                break;
        }
    }
    
    
    private:
    bool* date_is_satisfying(int y,int m,int d, bool date){//Checking if data is in border years(0 - 99/2024) months(0 - 11) days(0 - 30) returns a bool array with 3 values which stands for [y m d] with values true/false, if they are satisfying.
        const int SIZE = 3;
        bool* c = (bool*)malloc(SIZE*sizeof(bool));
        for(int i = 0; i<SIZE; i++) c[i] = true;
        
        const int MINVALUE = 0;
        int MAXMONTH;
        int MAXDAY;
        int MAXYEAR;
        if (date){
            MAXYEAR = 2024;
            MAXMONTH = 12;
            MAXDAY = 31;
        }else{
            MAXYEAR = 99;
            MAXMONTH = 11;
            MAXDAY = 30;
        }
        
        if (y > MAXYEAR || y < MINVALUE) c[0] = false;
        if (m > MAXMONTH || m < MINVALUE) c[1] = false;
        if (d > MAXDAY || d < MINVALUE) c[2] = false;
        
        return c;
    }


    void experience_error_handling(int* y,int* m,int* d, bool date){//Get the results from date_is_satisfying and check for false values.
        bool* date_test = date_is_satisfying(*y,*m,*d, date);
        while(!date_test[0]){
            if (date){
                std::cout<<"Years cannot be > 2024 and < 0\n"<<"Introduce the years again:\n";
            }else{
                std::cout<<"Years cannot be > 99 and < 0\n"<<"Introduce the years again:\n";
            }
            std::cin>>*y;
            date_test = date_is_satisfying(*y,*m,*d,date);
        }
            
            
        while(!date_test[1]){
            if (date){
                std::cout<<"Months cannot be > 12 and < 0\n"<<"Introduce the months again:\n";
            }else{
                std::cout<<"Months cannot be > 11 and < 0\n"<<"Introduce the months again:\n";
            }
            std::cin>>*m;
            date_test = date_is_satisfying(*y,*m,*d,date);
        }


        while(!date_test[2]){
            if (date){
                std::cout<<"Days cannot be > 31 and < 0\n"<<"Introduce the days again:\n";
            }else{
                std::cout<<"Days cannot be > 30 and < 0\n"<<"Introduce the days again:\n";
            }
            std::cin>>*d;
            date_test = date_is_satisfying(*y,*m,*d,date);
        }
    }
    

    pension_type pension_string_to_type(std::string str){
        std::string temp;
        for(auto c:str){
            temp+=tolower(c);
        }


        if (temp.find("acc")!= std::string::npos){
            return pension_type::ACCUMULATED;
        }
        else if (temp.find("dis")!= std::string::npos){
            return pension_type::DISABILITY;
        }
        else if (temp.find("sur")!= std::string::npos){
            return pension_type::SURVIVOR_LOSS;
        }
        else{
            return pension_type::UNDEFINED;
        }
    }


    payment_type payment_string_to_type(std::string str){
        std::string temp;
        for(auto c:str){
            temp+=tolower(c);
        }


        if (temp.find("con")!= std::string::npos)
        {
            return payment_type::CONSTANT;
        }
        else if (temp.find("dif")!= std::string::npos)
        {
            return payment_type::DIFFERENT;
        }
        else
        {
            return payment_type::UNDEFINED;
        }
    }


    bool time_period_is_satisfying(float time_period,float time){
        return time_period < time? true:false;
    }


    bool check_for_birth_day_expirience_correctness(){
        return (this->age - this->experience_in_years < 16 && this->experience_in_years != 0 )?false:true;
    }


    std::string vector_pension_survivor_as_string(){
        std::string temp = ""; 
        for (std::size_t i = 0; i < this->kormilici_pensions.size(); ++i) {
            std::stringstream x;
            x << "Pension [" << i + 1 << "]: " << kormilici_pensions[i];
            if(i!=this->kormilici_pensions.size()-1){ 
            x << ", ";
            }
            temp.append(x.str());
        }
        return temp;
        }


    std::string vector_sal_as_string(){
        std::string temp = ""; 
        for (std::size_t i = 0; i < this->sal.sum.size(); ++i) {
            std::stringstream x;
            x << "Salary [" << i + 1 << "]: " << this->sal.sum[i] << " Time: " << this->sal.time_period[i];
            if(i!=this->sal.sum.size()-1){
            x << ", ";
            }
            temp.append(x.str());
        }
    return temp;
    }
};  