let source

function toaster(t){
    msg.innerHTML=t;
    setTimeout(function () { msg.innerHTML="&nbsp;"; },30000);
}

function ajax(url,decode=true){
    document.body.style.cursor = "wait";
    toaster("&nbsp;");
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


function redgreen(n,b){
    let i=document.getElementById(n)
    i.classList.remove(b ? "led-red":"led-green");
    i.classList.add(b ? "led-green":"led-red");
}

function onofRender(b){
    let node=document.getElementById("onof")
    node.children[0].src=(b ? "on":"of")+".jpg"
}

function properCase(s){ return s.split(" ").map(function (x){ return x.charAt(0).toUpperCase() + x.slice(1) }).join(" ") }

function uiItem(d,h="uhang"){
    let parts=d.split(",");
    let n=parts[0]
    let node=document.getElementById(n)
    let v=parts[2]
    let b=parseInt(v);
    if(!node){
        let hangoff=document.getElementById(h);
        let t=parseInt(parts[1])
        let a=parseInt(parts[3])
        let title=document.createElement("div");
        title.innerHTML=properCase(n);
        let valu=document.createElement(t<4 ? "div":(t==4 ? "input":"select"));
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
                valu.classList=(a ? "uia ":"")+"ld led-"+(b==1 ? "green":"red");
                source.addEventListener(n, function(e){ redgreen(n,parseInt(e.data)); });
                if(a) {
                    valu.addEventListener("click", function(e){ 
                        ajax("h4/wifi/uichg/"+n+","+(valu.classList.value.indexOf("red")==-1 ? 0:1)) },
                        {capture: true});
                    title.classList.add("tuia");
                }
                break;
            case 4: // input
                valu.value=v;
                valu.addEventListener("blur", function(e){
                    if(valu.value!=v) {
                        v=valu.value;
                        ajax("h4/wifi/uichg/"+n+","+v.replace("http://",""))
                    }
                },{capture: true});
                title.classList.add("tuia");
                valu.classList.add("tuia");
                source.addEventListener(n, function(e){ document.getElementById(n).value=e.data; });
                break;
            case 5: // droplist
                let opts=parts.slice(2,parts.length-1)
                opts.sort()
                opts.forEach(function(e){
                    let kv=e.split("=");
                    let opt=document.createElement("option");
                    opt.value=kv[1];
                    opt.innerHTML=kv[0];
                    valu.appendChild(opt);
                })
                valu.addEventListener("change", function(e){ 
                    ajax("h4/wifi/uichg/"+n+","+valu.value) },
                    {capture: true});
                title.classList.add("tuia");
                valu.classList.add("tuia");
                break;
        }
        hangoff.insertBefore(title,null)
        hangoff.insertBefore(valu,null)
    }
    else {
        if(n=="onof"){
            let img=document.createElement("img");
            node.appendChild(img)
            node.style.display='block'
            onofRender(b);
            source.addEventListener(n, function(e){ onofRender(parseInt(e.data)) });
            node.addEventListener('click', function(e){ ajax("h4/toggle",false); }
            );
        }
    }
}

document.addEventListener("DOMContentLoaded", function() {
    if(!!window.EventSource) source=new EventSource("/evt");
    let rr=document.getElementById("reply")
    let cmd=document.getElementById("cmd")
    let msg=document.getElementById("msg")

    source.addEventListener('ui', function(e){ uiItem(e.data) });
    source.onmessage=function(e){ toaster(e.data) }
    source.onerror=function(e){ document.querySelectorAll('div.uv').forEach(function(d){ d.style.background="#ff0000" })}

    toaster("Thank you for using H4/Plugins - please support me on Patreon, see link below");
        
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
})