//let onof
let source

function toaster(t){
    msg.innerHTML=t;
    setTimeout(function () { msg.innerHTML="&nbsp;"; },30000);
}

function ajax(url,decode=true){
    document.body.style.cursor = "wait";
    var http = new XMLHttpRequest();
    var fullurl="http://"+window.location.hostname+"/rest/"+url;
    http.open('GET', fullurl);
    let r=document.getElementById("reply")
    http.addEventListener("load", function(e){
        document.body.style.cursor = "default";
        if(decode){
            r.innerHTML=""
            r.style.color="#ffffff" // css
            var j=JSON.parse(e.currentTarget.responseText);
            if(!j.res) j.lines.forEach(function(l){ r.innerHTML+=l+'\n'})
            else toaster("Error: "+j.res+" "+j.msg)
        }
    });
    http.send();  
}

function redgreen(id,b){
    let i=document.getElementById(id)
    i.classList.remove(b ? "led-red":"led-green");
    i.classList.add(b ? "led-green":"led-red");  
} 

function uiOnOff(b){
    let node=document.getElementById("onof")
    node.children[0].src=b ? "on.jpg":"of.jpg"
}

function uiItem(d,h="uhang"){
    let parts=d.split(",");
    let n=parts[0]
    let node=document.getElementById(n)
    let v=parseInt(parts[2])
    if(!node){
        let hangoff=document.getElementById(h);
        let t=parseInt(parts[1])
        let a=parseInt(parts[3])
        let title=document.createElement("div");
        title.innerHTML=n;
        let valu=document.createElement("div");
        valu.id=n;
        valu.className="uv";
        switch(t){
            case 0: // label
                valu.innerHTML=v;
                source.addEventListener(n, function(e){ document.getElementById(n).innerHTML=e.data; });
                break;
            case 1: // text
                break;
            case 2: // bool
                let b=parseInt(v);
                valu.classList=(a ? "uia ":"")+"ld led-"+(b ? "green":"red");
                source.addEventListener(n, function(e){ redgreen(n,parseInt(e.data)); });
                if(a) {
                    valu.addEventListener("click", function(e){ 
                        ajax("h4/asws/uib/"+n+","+(valu.classList.value.indexOf("red")==-1 ? 0:1)) },
                        {capture: true});
                    title.classList.add("tuia");
                }
                break;
        }
        hangoff.insertBefore(title,null)
        hangoff.insertBefore(valu,null)
    }
    else {
        if(n=="onof"){
            let img=document.createElement("img");
            node.appendChild(img)
            uiOnOff(v)
            node.style.display='block'
            node.addEventListener('click', function(e){ ajax("h4/toggle",false); });
            source.addEventListener(n, function(e){ uiOnOff(parseInt(e.data)) });
        }
    }
}

function has(id){ return document.getElementById("has"+id).value!="?" }

document.addEventListener("DOMContentLoaded", function() {
    if(!!window.EventSource) source=new EventSource("/evt");
    let rr=document.getElementById("reply")
    let cmd=document.getElementById("cmd")
    let msg=document.getElementById("msg")
    // frig ap test
    //wifimode=2
    if(parseInt(document.getElementById("wifi").value)==2) document.getElementById("ap").style.display='inline-grid'
    else {
        source.addEventListener('ui', function(e){ uiItem(e.data) });

        source.onmessage=function(e){
            let m=e.data;           
            if(m.substr(0,2)!='ka') toaster(e.data)
        }

        source.onmessage({data: "Thank you for using H4/Plugins - please support me on Patreon"});

        if(has("frnd")){
            frnd=document.getElementById("frnd");
            frnd.style.display='inline-flex'
        }

        document.getElementById("cc").addEventListener('submit', function(e){
            e.preventDefault();
            ajax(cmd.value,true);
        },{capture: true});

        rr.addEventListener('click', function(e){
            let lines=rr.value.split("\n")
            let n=rr.value.substr(0, rr.selectionStart).split("\n").length
            cmd.value=lines[n-1]
            ajax(lines[n-1],true)
        },{capture: true});
        document.getElementById("qh").addEventListener('click', function(e){ ajax("help",true) },{capture: true});
    } 
})