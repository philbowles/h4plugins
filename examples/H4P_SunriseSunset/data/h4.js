let onof
function ajax(url,decode=true){
    var http = new XMLHttpRequest();
    var fullurl="http://"+window.location.hostname+"/rest/"+url;
    http.open('GET', fullurl);
    let r=document.getElementById("reply")
    http.addEventListener("load", function(e){
        console.warn(e.currentTarget.responseText)
        if(decode){
            r.innerHTML=""
            r.style.color="#ffffff" // css
            var j=JSON.parse(e.currentTarget.responseText);
            if(!j.res) j.lines.forEach(function(l){ r.innerHTML+=l+'\n'})
            else {
                r.innerHTML="Error: "+j.res+" "+j.msg
                r.style.color="#d35400" // css
            }
        }
    });
    http.send();  
}
  
document.addEventListener("DOMContentLoaded", function() {
    let rr=document.getElementById("reply")
	let hasonof=parseInt(document.getElementById("hasonof").value);
	let wifimode=parseInt(document.getElementById("wifi").value);
    let cmd=document.getElementById("cmd")
    // frig ap test
    //wifimode=2
    if(wifimode==2) {
        document.getElementById("ap").style.display='inline-grid'
        document.getElementById("sta").style.display='none'
    }
    else {
        document.getElementById("ap").style.display='none'
        document.getElementById("sta").style.display='inline-grid'
       if(hasonof){
            onof=document.getElementById("onof");
            onof.parentNode.style.display='block'
            onof.addEventListener('click', function(e){ ajax("h4/toggle",false); });
            if(!!window.EventSource){
                source=new EventSource("/evt");
                source.addEventListener('onof', function(e){ onof.src=parseInt(e.data) ? "on.jpg":"of.jpg" });
            }        
        }
        document.getElementById("cc").addEventListener('submit', function(e){
            e.preventDefault();
            ajax(cmd.value,true);
        }),{capture: true};
        rr.addEventListener('click', function(e){
            let lines=rr.value.split("\n")
            let n=rr.value.substr(0, rr.selectionStart).split("\n").length
            cmd.value=lines[n-1]
            ajax(lines[n-1],true)
        }),{capture: true};
        document.getElementById("qh").addEventListener('click', function(e){ ajax("help",true) }),{capture: true};
    } 
})