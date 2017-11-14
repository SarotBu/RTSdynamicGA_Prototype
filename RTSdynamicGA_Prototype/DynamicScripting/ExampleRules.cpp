

void behaviour001(){
    if (mineral > 150){
        buildCommandCenter();
    }
}

void behaviour002(int param01){
    if (mineral > 250){
        buildMarine(param01);
    }
}

void main(){
    if (choice == 1){
        behaviour001();
    }else if (choice == 2){
        behaviour002(marineNum);
    }

}
