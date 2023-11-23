async function getPower() {
  const result = await fetch("/api/power");
  const power = await result.json();
  console.log(power);
  console.log(power.power);
  const el = document.getElementById("barrapower");
  el.innerText=power.power;
  var valore=parseInt(power.power);
  valore=valore/10;
  valore=Math.floor(valore)
  console.log(valore);
  var lunghezza=valore.toString();
  console.log(lunghezza);
  el.style.width=lunghezza.concat("%");

  el.style.width=valore;
}


async function getLight() {
  const result = await fetch("/api/light");
  const luce = await result.json();
  console.log(luce);
  console.log(luce.light);
  const el = document.getElementById("barralight");
  el.innerText="LUX";
  var valore=parseInt(luce.light);
  valore=valore/5;
  valore=Math.floor(valore)
  console.log(valore);
  valore=100-valore;
  var lunghezza=valore.toString();
  console.log(lunghezza);
  el.style.width=lunghezza.concat("%");

  el.style.width=valore;
}


function start(){
    getPower();
    getLight();
}
setInterval(start,250);
