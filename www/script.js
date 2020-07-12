
const kettle = document.querySelector('.kettle');
const temp = document.querySelector('.temp');
var water = 10;

var kettle_limit=0;
var kettle_empty=150;
var kettle_temp=10;
var kettle_on; 

function pull_IoT_data(){
    var xhttp = new XMLHttpRequest();

    xhttp.onreadystatechange = function() {
        if(this.responseText !== "" && this.status == 200){
            console.log(JSON.parse(this.responseText).Content);
        }
    };

    xhttp.open("GET", "/IoTdata");
    xhttp.send();
};

function update_water_level() {
    
    kettle.style.backgroundPosition = "0px "+String(water)+"px";
    if(water == kettle_empty || water == kettle_limit){}
    // else{water+=10;}
}

function display_temp(){
    console.log( temp.textContent);
    temp.textContent=String(kettle_temp)+" ".charCodeAt(176) +"C" ; 
}

var intervalID = setInterval(update_water_level, 1000);
var intervalID = setInterval(display_temp, 1000);
var intervalID = setInterval(pull_IoT_data,1000);