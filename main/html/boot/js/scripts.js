
let isLedOn = false;

async function toggleLedCheck() {
const el = document.getElementById("led");
isLedOn=!(led.checked);
console.log(led.checked);
fetch("api/led", { method: "POST", body: JSON.stringify({ isLedOn }) });

}

function showVal(newVal){
  const el = document.getElementById("percentuale");
    console.log(newVal);
    let conta= newVal-640;
    conta=Math.floor((conta/383)*100);
    var stringa=conta.toString();
    stringa=stringa.concat("%");
    el.innerText=stringa;
    fetch("api/fade", { method: "POST", body: JSON.stringify({ newVal }) });
}

let isAIon = false;
function toggleAI(){
  const el = document.getElementById("ailed");
  isAIon=(!ailed.checked);
    fetch("api/AI", { method: "POST", body: JSON.stringify({ isAIon }) });
}

let SSID;

function sendSSID(){
   const el = document.getElementById("msg");
    console.log(el.value);
    SSID=el.value;

    if(el.value.length>0){
      fetch("api/ssid", { method: "POST", body: JSON.stringify({ SSID }) });
    }

}

let wifiserver;
let passserver;

function sendServer(){
   const els = document.getElementById("wifiserver");
   const elp = document.getElementById("passserver");

    wifiserver=els.value;
    passserver=elp.value;
    console.log(wifiserver);
    console.log(passserver);

    if((elp.value.length>0)&&(els.value.length>0)){
      fetch("api/server", { method: "POST", body: JSON.stringify({ wifiserver, passserver }) });
    }

}
