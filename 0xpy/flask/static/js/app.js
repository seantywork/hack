

async function goFetch(){

    $('#app-img-switch').html('<img src="/static/img/super_cat_hunt.jpg" class="rounded-circle">')

    var query = $('#input-query').val() 

    var psw = $('#input-psw').val()

    var req_data = {"query":query,"psw":psw}

    var resp = await axios.post('/sendme', req_data)

    
    var resp_data = resp.data

    if (resp_data["status"] == 'n'){


        var message = resp_data["res"]

        var error_html = `
             
        <thead>
        <tr>
          <th>Error </th>
        </tr>
        </thead>
        <tbody>
         <tr>
           <td>${message}</td>
         </tr>
        </tbody>        
        
        `


        $('#app-result-render').html(error_html)


    } else {

        var records = resp_data["res"]


        var result_html  = `
        <thead>
          <tr>
            <th>ENTRY </th>
            <th> USAGE</th>
          </tr>
        </thead>
        <tbody id="app-result-render-body">
        </tbody>
        `


        $('#app-result-render').html(result_html)

        for (let i=0;i<records.length;i++){

            var entry = records[i]["ENTRY"]

            var usage = records[i]["USAGE"]

            var body_html = `
            <tr>
                <td> ${entry} </td>
                <td> ${usage} </td>
            </tr>
            
            `
            $('#app-result-render-body').append(body_html)

        }

        
    }

    $('#app-img-switch').html('<img src="/static/img/super_cat.jpg" class="rounded-circle">')
}

