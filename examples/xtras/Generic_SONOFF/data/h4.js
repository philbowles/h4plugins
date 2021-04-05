let source
let colortable=["red","orange","yellow","green","blue","bi"]

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

function ledonoff(n,c,b){
    let i=document.getElementById(n)
    let color="led-"+c+"-";
    i.classList.remove(color+(b ? 0:1));
    i.classList.add(color+b);
}

function onofRender(b){
    let node=document.getElementById("onof")
    node.children[0].src=(b ? "on":"of")+".jpg"
}

function properCase(s){ return s.split(" ").map(function (x){ return x.charAt(0).toUpperCase() + x.slice(1) }).join(" ") }

function options(elem,opts){
    elem.innerHTML="";
    opts.sort()
    opts.forEach(function(e){
        let kv=e.split("=");
        let opt=document.createElement("option");
        opt.value=kv[1];
        opt.innerHTML=kv[0];
        elem.appendChild(opt);
    })
}

function uiItem(d){
    let parts=d.split(",");
    console.warn("UI",d,parts);
    let n=parts[0]
    let node=document.getElementById(n)
    let v=parts[2]
    let b=parseInt(v);
    if(!node){
        let hangoff=document.getElementById(parts[parts.length-1]+"hang");
        let t=parseInt(parts[1])
        let a=parseInt(parts[3])
        let title=document.createElement("div");
        title.innerHTML=properCase(n);
        let valu=document.createElement(t<5 ? "div":(t==5 ? "input":"select"));
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
            case 3: // gpio
                let hue=colortable[parseInt(parts[4])];
                valu.classList="ld led-"+hue+"-"+b
                source.addEventListener(n, function(e){ ledonoff(n,hue,parseInt(e.data)) })
                /*
                if(a) {
                    valu.addEventListener("click", function(e){ 
                        ajax("h4/config/"+n+"="+(valu.classList.value.indexOf("1")==-1 ? 1:0)) },
                        {capture: true});
                    title.classList.add("tuia");
                }
                */
                break;
            case 5: // input
                valu.value=v;
                valu.addEventListener("blur", function(e){
                    if(valu.value!=v) {
                        v=valu.value;
                        ajax("h4/config/"+n+"="+v.replace("http://",""))
                    }
                },{capture: true});
                title.classList.add("tuia");
                valu.classList.add("tuia");
                source.addEventListener(n, function(e){ document.getElementById(n).innerHTML=e.data; });
                break;
            case 6: // droplist
                options(valu,parts.slice(2,parts.length-3))
                valu.addEventListener("change", function(e){ document.location="http://"+valu.value });
                title.classList.add("tuia");
                valu.classList.add("tuia");
                source.addEventListener(n, function(e){
                    let bits=e.data.split(",")
                    options(document.getElementById(n),bits)
                });
                break;
            default:
                break;
        }
        hangoff.insertBefore(title,null)
        hangoff.insertBefore(valu,null)
    }
    else {
        if(n=="onof"){
            let img=document.createElement("img");
            node.appendChild(img)
            node.style.display='inline-flex'
            onofRender(b);
            source.addEventListener(n, function(e){ 
                console.warn("RENDER ME BABY ",parseInt(e.data))
                onofRender(parseInt(e.data))
            });
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