

var FRONT_WS = {}

var READ_CHANNEL = ""

function FrontSockInit(){

    var or_url = window.location.href

    var proto_url = or_url.split('://') 

    var proto = proto_url[0]

    var urlbody_trail = proto_url[1].split('/')

    var urlbody = urlbody_trail[0]

    var trail_path = '/front-client'

    if (proto == 'http'){

        proto = 'ws://'

        var urlbody_port = urlbody.split(':')

        urlbody = urlbody_port[0] + ':' + '3000'

    } else if (proto == 'https'){

        proto = 'wss://'

        var urlbody_port = urlbody.split(':')

        urlbody = urlbody_port[0] 

    }

    var ws_url = proto + urlbody + trail_path

    console.log(ws_url)

    if (!REQ_KEY){
        alert("request key is empty")
        return {}
    }

    var ws = new WebSocket(ws_url)

    ws.onopen = function (){

        ws.send(REQ_KEY)

    }

    ws.onmessage = function (evnt){

        var resp = evnt.data

        READ_CHANNEL = resp

    }

    
    return ws

}

async function CheckReadChannelWithTimeOut(limit){


    var counter = 0


    while (counter < limit){

        if (READ_CHANNEL === ""){

            counter += 1 

        } else {
            console.log('read signal received')
            break
        }

        await Delay(1)

    }

    if (counter >= limit){

        alert('time limit exceeded')

        return 1

    }

    return 0

}


function Delay(time) {
    return new Promise(resolve => setTimeout(resolve, time));
}

async function SoliQuery(sock_query_el){

    var target_el = '#' + sock_query_el

    var sock_query = $(target_el).val()

    if(!sock_query){
        alert('feed correct query')
        return
    }


    READ_CHANNEL = ""

    FRONT_WS.send(sock_query)

    var ret_signal = await CheckReadChannelWithTimeOut(10000)

    if (ret_signal == 1){
        return
    }


    RenderOutput(sock_query, READ_CHANNEL)


}

FRONT_WS = FrontSockInit()

console.log('front sock ready')