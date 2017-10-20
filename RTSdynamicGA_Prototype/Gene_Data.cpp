
/*
    IN USED ->
    Behavior01();
    Behavior02();
    .
    .
    .
    BehaviorX();
    ================> These are genes
    Hence, We can store each gene as String "Behavior" + 'NUMBER'
*/

//gene[i] = "Behavior05();";

void Behavior01(){
    if (Mineral > 100){
        BuildSCV();
    }
}

void Behavior02(){
    if (Mineral > 150 && Barrack()){
        BuildMarine();
    }
}
