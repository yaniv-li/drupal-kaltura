/*
This file is part of the Kaltura Collaborative Media Suite which allows users 
to do with audio, video, and animation what Wiki platfroms allow them to do with 
text.

Copyright (C) 2006-2008  Kaltura Inc.

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as
published by the Free Software Foundation, either version 3 of the
License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

*/

// initModalBox called from gotoCW - to open the contribution wizard as an iFrame in the 
// widget page
function kalturaInitModalBox ( url, options )
{
	var objBody = document.getElementsByTagName("body").item(0);

	// create overlay div and hardcode some functional styles (aesthetic styles are in CSS file)
	var objOverlay = document.createElement("div");
	objOverlay.setAttribute('id','overlay');
	objBody.appendChild(objOverlay, objBody.firstChild);
	
	var width = 680;
	var height = 360;
	if (options)
	{
		if (options.width)
			width = options.width;
		if (options.height)
			height = options.height;
	}
	
	// create modalbox div, same note about styles as above
	var objModalbox = document.createElement("div");
	objModalbox.setAttribute('id','modalbox');
	//objModalbox.setAttribute('style', 'width:'+width+'px;height:'+height+'px;margin-top:'+(0-height/2)+'px;margin-left:'+(0-width/2)+'px;');
	objModalbox.style.width = width+'px';
	objModalbox.style.height = height+'px';
	objModalbox.style.marginTop = (0-height/2)+'px';
	objModalbox.style.marginLeft = (0-width/2)+'px';
	
	// create content div inside objModalbox
	var objModalboxContent = document.createElement("div");
	objModalboxContent.setAttribute('id','mbContent');
	if ( url != null )
	{
		objModalboxContent.innerHTML = '<iframe scrolling="no" width="' + width + '" height="' + height + '" frameborder="0" src="' + url + '"/>';
	}
	objModalbox.appendChild(objModalboxContent, objModalbox.firstChild);
	
	objBody.appendChild(objModalbox, objOverlay.nextSibling);	
	
	return objModalboxContent;
}
function SendTopToNodePage(){
	window.top.location.href = node_url;
}
function SendTopToEntriesPage(){
  window.top.location.href = goto_url;
}
function kalturaCloseModalBox ()
{
	if ( this != window.top )
	{
		window.top.kalturaCloseModalBox();
		return false;
	}
	//alert ( "kalturaCloseModalBox" );
	// TODO - have some JS to close the modalBox without refreshing the page if there is no need
	overlay_obj = document.getElementById("overlay");
	modalbox_obj = document.getElementById("modalbox");
	overlay_obj.parentNode.removeChild( overlay_obj );
	modalbox_obj.parentNode.removeChild( modalbox_obj );
	
	return false;
}

function $id(x){ return document.getElementById(x); }


function kalturaRefreshTop ()
{
	if ( this != window.top )
	{
		window.top.kalturaRefreshTop();
		return false;
	}	
	window.location.reload(true);
}

function switch_to_exist_partner() {
	var href = location.href;
	tmp = href.replace('&register=no', '');
	href = tmp.replace('?register=no', '');
	if(href.indexOf('?') > 0)
		location.href = href + '&register=no'
	else
		location.href = href + '?register=no'
}

function switch_to_register() {
	var href = location.href;
	tmp = href.replace('&register=no', '');
	href = tmp.replace('?register=no', '');
	location.href = href;
}

KalturaThumbRotator = {

	slices : 16, // number of thumbs per video
	frameRate : 1000, // frameRate in milliseconds for changing the thumbs
	
	timer : null,
	slice : 0,
	img  : new Image(),
	
	thumbBase : function (o) // extract the base thumb path by removing the slicing parameters
	{
		var path = o.src;
		var pos = path.indexOf("/vid_slice");
		if (pos != -1)
			path = path.substring(0, pos);
			
		return path;
	},
	

	change : function (o, i) // set the Nth thumb, request the next one and set a timer for showing it
	{
		slice = (i + 1) % this.slices;

		var path = this.thumbBase(o);
		
		o.src = path + "/vid_slice/" + i + "/vid_slices/" + this.slices;
		this.img.src = path + "/vid_slice/" + slice + "/vid_slices/" + this.slices;

		i = i % this.slices;
		i++;
		
		this.timer = setTimeout(function () { KalturaThumbRotator.change(o, i) }, this.frameRate);
	},
	
	start : function (o) // reset the timer and show the first thumb
	{
		clearTimeout(this.timer);
		var path = this.thumbBase(o);
		this.change(o, 1);
	},

	end : function (o) // reset the timer and restore the base thumb
	{
		clearTimeout(this.timer);
		o.src = this.thumbBase(o);
	}
};


function remove_items_from_field(field_id) { document.getElementById(field_id).value = ''; }
function get_title() { return document.getElementById("edit-title").value; }

function kaltura_activate_player(thumb_div, player_div) {
	document.getElementById(thumb_div).style.display = 'none';
	document.getElementById(player_div).style.display = 'block';
}