const express = require('express');
const fs = require('fs');
var tableyestaday = {};
var tabletoday = {};
var alldevice ={};	//所有的设备
var alldevicebyday = {};	//所有的设备按天整理
var allnewdevicebyday = {};	//所有的新设备数量按天整理
var onlinenumbers = {}	;	//时间为key 的在线人数	时间单位 为分钟 起始点 2017年的1月1号
var today = {};			//今天的登陆设备
var currentonline = {}		//当前的在线设备
var oldday;
var todaynewdevice = 0;

var userIDstoday = {};
var userCountHistroy =[] ;
var newUserCountHistroy =[] ;
var todaynewuserCount = 0
var alluserIDs = {};
var todaynewuser = {};

main()

function main ()
{
	Load();
	var app = express();
	app.set('view engine','ejs');
	app.use(require('body-parser').urlencoded());
	app.use(require('body-parser').raw());
	app.use('/static',express.static(__dirname + '/static'));
	app.post("/ServerDeviceInfo",(req ,res) =>
	{
		if(req.body["DeviceID"])
		{
			RegDevice(req.body["DeviceID"]);			
		}
		var d = new Date()
		var month = d.getMonth() + 1;
		var Year = d.getFullYear();
		var daystr = Year.toString() + "_"+ month.toString() + "_" + d.getDate();				
		res.send(daystr);
	})
	app.post("/ServerDeviceInfo2",(req ,res) =>
	{
		if(req.body["DeviceID"])
		{
			RegDevice(req.body["DeviceID"]);			
		}
		var d = new Date()
		var month = d.getMonth() + 1;
		var Year = d.getFullYear();
		var daystr = Year.toString() + "_"+ month.toString() + "_" + d.getDate()+ "|" + req.ip;				
		res.send(daystr);
	})
	app.post("/ServerDeviceInfo3",(req ,res) =>
	{
		
		if(req.body["DeviceID"])
		{
			RegDevice(req.body["DeviceID"]);			
		}
		if(req.body["UserID"])
		{
			RegUser(req.body["UserID"]);			
		}
		
		var d = new Date()
		var month = d.getMonth() + 1;
		var Year = d.getFullYear();
		var daystr = Year.toString() + "_"+ month.toString() + "_" + d.getDate()+ "|" + req.ip;				
		res.send(daystr);
	})
	app.get("/info",(req,res) =>
	{
		var outstr = ("<html>");  
		outstr += ("<head>");  

		outstr += 	("<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\">");  		
		outstr += 	("<title>xxooxx</title>");  
		outstr += 	("</head>");  
   		outstr += 	("<body>");  
   		outstr += 	(GetInfo()); 
   		outstr += 	(GetTableInfo()); 				
   		outstr += 	(GetCanvaseStrFirst());  
   		outstr += 	(GetCanvaseStrLine());  
   		outstr += 	(GetCanvaseStrLast());  
   		outstr += 	("</body>");  
   		outstr += 	("</html>");  
		res.end(outstr);
	})
	app.get('/info2',(req,res)=>{
		var view_onlinedata = [];//getOnlineNumberByDate(new Date());
		for(var i=0;i<7;++i)
		{
			var day = new Date();
			day = day - i * 24 * 60 * 60 * 1000;
			day = new Date(day);
			view_onlinedata.push({
				label:day.getFullYear()+'/' + (day.getMonth()+1) + '/' + (day.getDate()) + '('+(i==0?'今天':i+'天前')+')',
				data:getOnlineNumberByDate(day)
			});

		}
		view_onlinedata = view_onlinedata.reverse();
		res.render('index',{headers:GetInfo() + GetTableInfo(),view_onlinedata:view_onlinedata});
	});
	app.post("/PanicLog",(req,res) =>
	{
		console.log("PanicLog")			
			var d = new Date()
			var month = d.getMonth() + 1;
			var Year = d.getFullYear();
			var daystr = Year.toString() + "_"+ month.toString() + "_" + d.getDate() + "_" + d.getHours() + "_" + d.getMinutes() + "_"+ d.getSeconds() + "_"+ Date.now();				
			const writable = fs.createWriteStream(__dirname + "/PanicLog/" + daystr + ".txt");
//			writable.write("ip : " + request.socket.remoteAddress + '\r\n');
			console.log(req.body)			
			writable.write(req.body['hash error.frame']);
			writable.end();
			res.send(200);
	})
	//需要记录的数据 各种桌子的数量 加载所使用的时间 
	app.post("/StartLoad",(req,res) =>
	{
		var tableinfo ={} ;
		tableinfo.PlayerID = req.body.DeviceID;
		tableinfo.TableID = req.body.table;
		tableinfo.Map = req.body.Map
		tableinfo.skinids = req.body.skinids;	
		tableinfo.Starttime = Date.now();		
		tableinfo.LoadOverTime = -1;
		RegTableInfoStart(tableinfo);		
		//req.body = 
		res.sendStatus(200);
	})
	
	app.post("/LoadOver",(req,res) =>
	{
		var tableid = req.body.table;
		var PlayerID = req.body.DeviceID;
		RegTableInfoEnd(tableid+PlayerID,req.body.LoadOverTime);		
		//req.body = 
		res.sendStatus(200);
	})
	
	var server = app.listen(8502, function () {
	var host = server.address().address;
	var port = server.address().port;

  console.log('server app listening at http://%s:%s', host, port);
});
	
	 
	console.log('server started');  
	UpdateDevice();
	var timeout = setInterval(function() {
                            UpdateDevice();
                   }, 1000);
	var timeout = setInterval(function() {
			OnLineCount();
   }, 60 * 1000);
	var savetime = setInterval(function(){
		SaveFunction();
	},30 * 60 * 1000);	
	console.log(process.memoryUsage());
	process.on('SIGINT', () => {
	console.log('Received SIGINT.  Save and to Exit');
	console.log('11111');
	Save();
	console.log('Save Over');
	process.exit(0);
});
}

function RegTableInfoStart(tableinfo)
{
	tabletoday[tableinfo.TableID + tableinfo.PlayerID] = tableinfo;		
}

function RegTableInfoEnd(tabledevicename,loadover)
{
	var thetable = tabletoday[tabledevicename];
	if(thetable)
	{
		thetable.LoadOverTime = loadover;
	}	
}

function RegUser(userID)
{
	userIDstoday[userID] = "";
	if(alluserIDs[userID] == "")
	{

	}
	else
	{
		alluserIDs[userID] = "";	
		todaynewuser[userID] = "";
		if(parseInt(userID) > 1068000)
		{
			todaynewuserCount++			
		}		
	}
}


function OnLineCount()
{
	var d = new Date()
	var e = Math.floor((d - new Date("2017,1,1")) / (60 *1000));
	onlinenumbers[e] = Object.keys(currentonline).length;
}

function getOnlineNumberByDate(day)
{
	
	var startDay = new Date(day);
	startDay.setHours(0,0,0,0,0);
	var startValue = Math.floor((startDay - new Date('2017,1,1')) / (60 * 1000));
	var arr = [];
	for(var i=startValue;i<startValue + 60 * 24;++i)
	{
		var n = onlinenumbers[i];
		if (typeof n === 'number')
			arr.push(n);
		else
			arr.push(null);
	}
	//删除结尾的null
	while(arr.length > 0 && arr[arr.length - 1] === null) arr.pop();
	//替换null，为之前的一个数字
	var last = 0;
	for(var i=0;i<arr.length;++i)
	{
		if (arr[i] === null)
			arr[i] = last;
		else
			last = arr[i];
	}
	return arr;
}


function RegDevice(devicename )
{
//	console.log(devicename );
	if(currentonline[devicename] == null)
	{
		currentonline[devicename] =  Date.now();
	}
	else
	{
		currentonline[devicename] =  Date.now();
	}
	today[devicename] = "";
	if(alldevice[devicename] == "")
	{
	}
	else
	{
		todaynewdevice = todaynewdevice + 1; 
		alldevice[devicename] = ""; 
	}
}

function NewDay(day)
{
	console.log("NewDay");
	if(today)
	{	
		alldevicebyday[day] = today 
		allnewdevicebyday[day] = todaynewdevice;
		SaveObj(todaynewdevice,"history/" + day.toString() + "_todaynewdevice")
		SaveObj(today,"history/" + day.toString() + "_todaydevice")

		todaynewdevice = 0;
		today = {};
		SaveObj(tabletoday,"history/" + day.toString() + "_tables")
		tableyestaday = tabletoday;
		tabletoday = {};

		SaveObj(userIDstoday,"history/" + day.toString() + "_userIDs")
		SaveObj(todaynewuser,"history/" + day.toString() + "_newuserIDs")
		
		console.log(userIDstoday);
		userCountHistroy[day] =   Object.keys(userIDstoday).length.toString()
		newUserCountHistroy[day] =  todaynewuserCount
		todaynewuserCount = 0;
		userIDstoday = {};
	}
}

function UpdateDevice()
{
	var d = new Date()
	var nowday = d.getDate();
	if(nowday != oldday)
	{
		var e = Math.floor((Date.parse(d) - Date.parse(new Date("2017,1,1")) )/ (24 * 3600 *1000));
		console.log(e);		
		NewDay(e);
		oldday = nowday;
	}

	var currenttime = Date.now();
	var toremove = [];
	if(currentonline )
	{
		Object.keys(currentonline).forEach(function(element)
		{
			if(currenttime  - currentonline[element] > 5 * 61 * 1000)
			{
				toremove.push(element) ;
			}
		});
	}
	if(toremove)
	{
		toremove.forEach(function(element)
		{
			delete  currentonline[element];
		})
	}
}

function GetTableInfo()
{	
	var string = "";
	var mapnamekey = {};
	Object.keys(tabletoday).forEach(function(element)
	{		
		if(mapnamekey[tabletoday[element].Map])
		{
			mapnamekey[tabletoday[element].Map].push(tabletoday[element]);
		}
		else
		{
			mapnamekey[tabletoday[element].Map] = [];				
			mapnamekey[tabletoday[element].Map].push(tabletoday[element]);
		}
	});
	var ct = Date.now()
	Object.keys(mapnamekey).forEach(function(element)
	{
		var totalcount = 0;
		var donecount = 0;
		var totalloadtime = 0.0;
		var themap = mapnamekey[element];
		Object.keys(themap).forEach(function(element2)
		{
			if(themap[element2].LoadOverTime != -1)
			{
				totalcount ++;
				donecount++;
				totalloadtime += parseFloat(themap[element2].LoadOverTime);
			}
			else if(themap[element2].LoadOverTime  == -1 && ct - themap[element2].Starttime > 300 * 1000)	//5分钟 之内都没有加载完成的话 就算没有加载完成
			{
				totalcount ++ ;
			}
		})
		if(donecount != 0)
		{
			var sss = "地图 : " + element + " 创建 "+ totalcount + "次 完成加载 " + donecount + " 次 " + " 平均加载时间 " + totalloadtime / donecount	+ "</br>"			
			string += sss;
		}		
	});
	return string;
}

function GetInfo()
{
	var d = new Date()
	var nowday = d.getDate();
	var e = Math.floor((Date.parse(d) - Date.parse(new Date("2017,1,1")) )/ (24 * 3600 *1000));
	console.log("昨天: " + e);
	if(alldevicebyday[e] == null)
	{
		alldevicebyday[e] = [];
	}
	if(alldevicebyday[e - 1] == null)
	{
		alldevicebyday[e - 1] = [];
	}
	var todaystr = "今日登陆设备:" + Object.keys(today).length.toString() + " 昨日登陆设备 : " + Object.keys(alldevicebyday[e]).length.toString() + " 前天登陆设备" + Object.keys(alldevicebyday[e - 1]).length.toString() +"<br>" + 
	" 当前在线:" + Object.keys(currentonline).length.toString() + "<br>" +
	" 今日登陆的新设备 :" + todaynewdevice + " 昨日登陆的新设备 " +  allnewdevicebyday[e] + " 前天登陆的新设备 " +  allnewdevicebyday[e - 1] + "<br>" + 
	" 总登陆设备:" + Object.keys(alldevice).length.toString() +  "<br>"  +
	" 今日登录用户数: " + Object.keys(userIDstoday).length.toString() + " 昨日登录用户数 : " + userCountHistroy[e] + " 前天登录用户数 "+ userCountHistroy[e - 1] + "<br>" +
	" 今日登录新用户数: " + todaynewuserCount + " 昨日登录新用户数 : " + newUserCountHistroy[e] + " 前天登录新用户数 "+ newUserCountHistroy[e - 1] + "<br>";
	
	//var yestoday = "昨天登陆设备:" + Object.keys(alldevicebyday[e ]).length.toString() + "<br>" +  " 昨天登陆的新设备 :" + allnewdevicebyday[e + 1] + "<br>" ;
	//var yestoday = "昨天登陆设备:" + Object.keys(alldevicebyday[e - 1]).length.toString() + "<br>" +  " 昨天登陆的新设备 :" + allnewdevicebyday[e + 1] + "<br>" ;
	//var yesyestoday = "前天登陆设备:" + Object.keys(alldevicebyday[e - 2]).length.toString() + "<br>" +  " 前天登陆的新设备 :" + allnewdevicebyday[e] + "<br>" ;
	return todaystr;
}

function GetCanvaseStrFirst()
{
	return `
	<canvas id="myCanvas" width="1600" height="800" style="border:1px solid #c3c3c3;">
您的浏览器不支持 HTML5 canvas 标签。
</canvas>

<script>

var d=document.getElementById("myCanvas");
var c=d.getContext("2d");
c.fillStyle="#c0c0c0";
c.fillRect(0,0,1600,800);

	`
}

function GetCanvaseStrLine()
{
	  var values = [];  
    for(var pro in onlinenumbers){  
      values.push(onlinenumbers[pro]);  
   }  
   	var script = "";
	var maxcount = Math.max.apply( Math,values );	//如果小于5000 则对齐到 500  如果小于20000 则对齐到1000 如果小于100000 对齐到 2000
	var step = 500;	
	if(maxcount < 5000)
	{
		step = 500;
		
	}
	else if(maxcount <  20000)
	{
		step = 1000;		
	}
	else
	{
		step = 2000;				
	}	
	var n = Math.ceil((maxcount * 1.05)/step) ;
	maxcount = step * n;
	script += DrawAx(maxcount,step,n);

	//总高是 800  下面 留 150是 坐标轴  上部有效高度 为 	650  0 的数量对应高度为650 maxcount 的对应的Y 为0  650 - count / maxcount * 650
	var d = new Date()
	var e = Math.floor((Date.parse(d) - Date.parse(new Date("2017,1,1")) )/ (24 * 3600 *1000));	//e 是今天的天数
	//得到今天的时间  
	//最近的 7天的数据
	var thelast7 = [];
	var daycolor = [];
	daycolor[0] = "#ff3030";
	daycolor[1] = "#e23afE";
	daycolor[2] = "#8a2be2";
	daycolor[3] = "#0a0a0a";
	daycolor[4] = "#008b00";
	daycolor[5] = "#caff70";
	daycolor[6] = "#0a008B";
	daycolor[7] = "#ff3030";
	//	var today = [];	//今天按分钟排序的数组
	for(var i = 7 ;i >= 0 ;i--)
	{
		var dayarray = [];
		thelast7[i] = dayarray;
		Object.keys(onlinenumbers).forEach(function(element)
		{
			if(element > (e - i) * 24 * 60 && element < (e + 1 - i) * 24 * 60 )
			{
				thelast7[i][element - (e - i)* 24 * 60] = onlinenumbers[element];
			}
		})	
		var isfirst = true;
		var count = 0;
		var value = 0;
		thelast7[i].forEach( (element,index)=>
		{
			if(isfirst)
			{
				script += 'c.beginPath()' + '\n';
				script += 'c.strokeStyle="' + daycolor[i] +'"' + '\n'
				if(i == 0 || i == 1 || i == 3 || i == 2)
				{
					script += 'c.lineWidth = 5 - ' + i + '\n';
				}
				else
				{
					script += 'c.lineWidth = 1' + '\n';					
				}
				script += "c.moveTo(150," + Math.floor(650 - element * 650 / maxcount ) + ")" + '\n';
				isfirst = false;
			}
			count = count + 1;
			value = value + element;
			totestcount = 10;
			if( i == 0)
			{
				totestcount = 2;
			}
			if( i == 1)
			{
				totestcount = 5;
			}
			if(count == totestcount)
			{
			//算5分钟的平均值 
				script += "c.lineTo(" + (index + 150).toString() + "," + Math.floor(650 - (value / count) * 650 / maxcount ) + ")" + '\n';		
				count = 0;
				value = 0;
			}
		}
		)
		script += 'c.stroke()' + '\n'
	}
	for(var i = 0 ; i < 25 ;i++)
	{
		if( i % 3 == 0)
		{
			script += "c.moveTo(" + (150 + i * 60) + "," + 700 + ")" + '\n';			
		}
		else
		{
			script += "c.moveTo(" + (150 + i * 60) + "," + 720 + ")" + '\n';		
		}
		script += "c.lineTo(" + (150 + i * 60) + "," + 750 + ")" + '\n';
	}
	return script;
}

function DrawAx(maxcount ,step,stepcount)
{
    var script = "";
	script += 'c.fillStyle="#000000"' + '\n'
	script += 'c.font= "30px Arial"' + '\n'	
	script += 'c.strokeStyle="#000000"' + '\n'
	script += 'c.beginPath()' + '\n';
	for(var i = 1 ; i <= stepcount ;i++)
	{
		script += "c.moveTo(120," + Math.floor(650 - step * i * 650 / maxcount ) + ")" + '\n';			
		script += "c.lineTo(150," + Math.floor(650 - step * i * 650 / maxcount ) + ")" + '\n';	
		script += 'c.fillText("' + i * step + '",40,' + (650 - step * i * 650 / maxcount + 15) + ')' + '\n'		
	}
	
	
	script += "c.moveTo( 150 ,650)" + '\n'
	script += "c.lineTo( 1600 ,650)" + '\n'
	script += "c.moveTo( 150 ,650)" + '\n'
	script += "c.lineTo( 150 ,0)" + '\n'
	script += "c.moveTo( 150 ,750)" + '\n'
	script += "c.lineTo( 1600 ,750)" + '\n'

	script += 'c.fillText("3:00",300,780)' + '\n'
	script += 'c.fillText("6:00",480,780)' + '\n'
	script += 'c.fillText("9:00",660,780)' + '\n'
	script += 'c.fillText("12:00",840,780)' + '\n'
	script += 'c.fillText("15:00",1020,780)' + '\n'
	script += 'c.fillText("18:00",1200,780)' + '\n'
	script += 'c.fillText("21:00",1380,780)' + '\n'
	script += 'c.fillText("24:00",1530,780)' + '\n'
	script += 'c.stroke()' + '\n'
	var daycolor = [];
	daycolor[0] = '"#ff3030"';
	daycolor[1] = '"#e23afE"';
	daycolor[2] = '"#8a2be2"';
	daycolor[3] = '"#0a0a0a"';
	daycolor[4] = '"#008b00"';
	daycolor[5] = '"#caff70"';
	daycolor[6] = '"#0a008B"';
	daycolor[7] = '"#ff3030"';
	for(var i = 0 ; i <= 7;i++)
	{
		script += 'c.fillStyle = ' + daycolor[i] + '\n';
		var d = new Date();
		d = new Date( d - 1000*60*60*24 * i);
		
		var theday = (d.getMonth() + 1) + '_' + d.getDate();
		script += 'c.fillText("' + theday + '",' + (150 + i * 180 + 25)  + ',690)' + '\n'
		script += 'c.fillRect(' + (150 + i * 180) + ',670,20,20);' + '\n';		
	}
	return script;

}




function GetCanvaseStrLast()
{
		return`c.stroke();
</script>`
}


function SaveFunction()
{
	Save();
}

function Save()
{
	console.log('Save Over');
	const fs = require('fs');
	
	var saveObject ={};
	saveObject.oldday = oldday;
	saveObject.alldevice = alldevice;
	saveObject.alldevicebyday = alldevicebyday;
	saveObject.onlinenumbers = onlinenumbers;
	saveObject.today = today;
	saveObject.currentonline = currentonline;	
	saveObject.todaynewdevice = todaynewdevice;		
	saveObject.allnewdevicebyday = allnewdevicebyday;		
	saveObject.tabletoday = tabletoday;		
	saveObject.tableyestaday = tableyestaday;
	
	
	saveObject.userIDstoday = userIDstoday;
	saveObject.userCountHistroy =userCountHistroy ;
	saveObject.newUserCountHistroy = newUserCountHistroy ;
	saveObject.todaynewuserCount = todaynewuserCount
	saveObject.alluserIDs = alluserIDs;	
	saveObject.todaynewuser = todaynewuser;
	var tosavevalue = JSON.stringify(saveObject);
	console.log("1231231");
	timestr = Date.parse(new Date()).toString();
	fs.writeFileSync(__dirname + "/" + timestr + ".bzs.txt", tosavevalue);
}

function SaveObj(obj,pathname)
{
	const fs = require('fs');
	var tosavevalue = JSON.stringify(obj);
	fs.writeFileSync(__dirname + "/" + pathname + ".bzs.txt", tosavevalue);	
}

//内存中保留近3天的数据  其他的数据 
function Load()
{
	const fs = require('fs');
	console.log(__dirname);
	//first find all files
	var data = fs.readdirSync(__dirname);
	var maxtime = 0;
	data.forEach(function(element)
	{
		if(element.lastIndexOf(".bzs.txt") != -1)
		{
			var a = element.substring(0,element.lastIndexOf(".bzs.txt"));
			var c = Number(a)	;
			if(c > maxtime)
			{
				maxtime = c;
			}
		}
	});
	if(maxtime != 0)
	{
		console.log("LoadFrom : " + maxtime.toString());
		var thestring = fs.readFileSync(__dirname + "/" + maxtime.toString() +  ".bzs.txt",'utf8');
		var obj2 = JSON.parse(thestring); 
		oldday = obj2.oldday;
		alldevice = obj2.alldevice;
		alldevicebyday = obj2.alldevicebyday;
		onlinenumbers = obj2.onlinenumbers;
		today = obj2.today;
		currentonline = obj2.currentonline;	
		todaynewdevice = obj2.todaynewdevice;
				
		userIDstoday = obj2.userIDstoday;
		if(!userIDstoday)
		{
			userIDstoday = {}
		}
		userCountHistroy = obj2.userCountHistroy ;
		if(!userCountHistroy)
		{
			userCountHistroy = []
		}
		newUserCountHistroy = obj2.newUserCountHistroy ;
		if(!newUserCountHistroy)
		{
			newUserCountHistroy = [];
		}

		todaynewuserCount = obj2.todaynewuserCount
		if(!todaynewuserCount)
		{
			todaynewuserCount = 0
		}
		alluserIDs = obj2.alluserIDs;
		if(!alluserIDs)
		{
			alluserIDs = {};
		}
		todaynewuser = obj2.todaynewuser;		
		if(!obj2.todaynewuser)
		{
			todaynewuser = {};			
		}
		if(obj2.allnewdevicebyday)
		{
			allnewdevicebyday = obj2.allnewdevicebyday;			
		}
		if(obj2.tabletoday)
		{	
			tabletoday = obj2.tabletoday;			
		}
		if(obj2.tableyestaday)
		{
			tableyestaday = obj2.tableyestaday;
		}
		alldevicetemp = {};
		Object.keys(alldevice).forEach(function(element2){
			alldevicetemp[element2] = "";
		});
		alldevice = alldevicetemp;
		alldaytemp = {};
		Object.keys(alldevicebyday).forEach(function(element2)
		{
			alldaytemp[element2] = {};
			Object.keys(alldevicebyday[element2] ).forEach(function(element3)
			{
				alldaytemp[element2][element3] = ""; 
			})
		});
		alldevicebyday = alldaytemp;		
	}		
}

