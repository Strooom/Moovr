<?xml version="1.0" encoding="utf-8"?>
<!DOCTYPE eagle SYSTEM "eagle.dtd">
<eagle version="8.1.1">
<drawing>
<settings>
<setting alwaysvectorfont="yes"/>
<setting verticaltext="up"/>
</settings>
<grid distance="0.1" unitdist="inch" unit="inch" style="lines" multiple="1" display="no" altdistance="0.01" altunitdist="inch" altunit="inch"/>
<layers>
<layer number="1" name="Top" color="4" fill="1" visible="no" active="no"/>
<layer number="2" name="Route2" color="1" fill="3" visible="no" active="no"/>
<layer number="3" name="Route3" color="4" fill="3" visible="no" active="no"/>
<layer number="4" name="Route4" color="1" fill="4" visible="no" active="no"/>
<layer number="5" name="Route5" color="4" fill="4" visible="no" active="no"/>
<layer number="6" name="Route6" color="1" fill="8" visible="no" active="no"/>
<layer number="7" name="Route7" color="4" fill="8" visible="no" active="no"/>
<layer number="8" name="Route8" color="1" fill="2" visible="no" active="no"/>
<layer number="9" name="Route9" color="4" fill="2" visible="no" active="no"/>
<layer number="10" name="Route10" color="1" fill="7" visible="no" active="no"/>
<layer number="11" name="Route11" color="4" fill="7" visible="no" active="no"/>
<layer number="12" name="Route12" color="1" fill="5" visible="no" active="no"/>
<layer number="13" name="Route13" color="4" fill="5" visible="no" active="no"/>
<layer number="14" name="Route14" color="1" fill="6" visible="no" active="no"/>
<layer number="15" name="Route15" color="4" fill="6" visible="no" active="no"/>
<layer number="16" name="Bottom" color="1" fill="1" visible="no" active="no"/>
<layer number="17" name="Pads" color="2" fill="1" visible="no" active="no"/>
<layer number="18" name="Vias" color="2" fill="1" visible="no" active="no"/>
<layer number="19" name="Unrouted" color="6" fill="1" visible="no" active="no"/>
<layer number="20" name="Dimension" color="15" fill="1" visible="no" active="no"/>
<layer number="21" name="tPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="22" name="bPlace" color="7" fill="1" visible="no" active="no"/>
<layer number="23" name="tOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="24" name="bOrigins" color="15" fill="1" visible="no" active="no"/>
<layer number="25" name="tNames" color="7" fill="1" visible="no" active="no"/>
<layer number="26" name="bNames" color="7" fill="1" visible="no" active="no"/>
<layer number="27" name="tValues" color="7" fill="1" visible="no" active="no"/>
<layer number="28" name="bValues" color="7" fill="1" visible="no" active="no"/>
<layer number="29" name="tStop" color="7" fill="3" visible="no" active="no"/>
<layer number="30" name="bStop" color="7" fill="6" visible="no" active="no"/>
<layer number="31" name="tCream" color="7" fill="4" visible="no" active="no"/>
<layer number="32" name="bCream" color="7" fill="5" visible="no" active="no"/>
<layer number="33" name="tFinish" color="6" fill="3" visible="no" active="no"/>
<layer number="34" name="bFinish" color="6" fill="6" visible="no" active="no"/>
<layer number="35" name="tGlue" color="7" fill="4" visible="no" active="no"/>
<layer number="36" name="bGlue" color="7" fill="5" visible="no" active="no"/>
<layer number="37" name="tTest" color="7" fill="1" visible="no" active="no"/>
<layer number="38" name="bTest" color="7" fill="1" visible="no" active="no"/>
<layer number="39" name="tKeepout" color="4" fill="11" visible="no" active="no"/>
<layer number="40" name="bKeepout" color="1" fill="11" visible="no" active="no"/>
<layer number="41" name="tRestrict" color="4" fill="10" visible="no" active="no"/>
<layer number="42" name="bRestrict" color="1" fill="10" visible="no" active="no"/>
<layer number="43" name="vRestrict" color="2" fill="10" visible="no" active="no"/>
<layer number="44" name="Drills" color="7" fill="1" visible="no" active="no"/>
<layer number="45" name="Holes" color="7" fill="1" visible="no" active="no"/>
<layer number="46" name="Milling" color="3" fill="1" visible="no" active="no"/>
<layer number="47" name="Measures" color="7" fill="1" visible="no" active="no"/>
<layer number="48" name="Document" color="7" fill="1" visible="no" active="no"/>
<layer number="49" name="Reference" color="7" fill="1" visible="no" active="no"/>
<layer number="51" name="tDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="52" name="bDocu" color="7" fill="1" visible="no" active="no"/>
<layer number="90" name="Modules" color="5" fill="1" visible="yes" active="yes"/>
<layer number="91" name="Nets" color="2" fill="1" visible="yes" active="yes"/>
<layer number="92" name="Busses" color="1" fill="1" visible="yes" active="yes"/>
<layer number="93" name="Pins" color="2" fill="1" visible="no" active="yes"/>
<layer number="94" name="Symbols" color="4" fill="1" visible="yes" active="yes"/>
<layer number="95" name="Names" color="7" fill="1" visible="yes" active="yes"/>
<layer number="96" name="Values" color="7" fill="1" visible="yes" active="yes"/>
<layer number="97" name="Info" color="7" fill="1" visible="yes" active="yes"/>
<layer number="98" name="Guide" color="6" fill="1" visible="yes" active="yes"/>
</layers>
<schematic xreflabel="%F%N/%S.%C%R" xrefpart="/%S.%C%R">
<libraries>
<library name="Mooov2">
<packages>
<package name="CONN4B">
<pad name="VCC" x="0" y="0" drill="1.4" diameter="3.048" shape="square" first="yes"/>
<pad name="A" x="0" y="-5.08" drill="1.4" diameter="3.048" shape="octagon"/>
<pad name="B" x="0" y="-10.16" drill="1.4" diameter="3.048" shape="octagon"/>
<pad name="GND" x="0" y="-15.24" drill="1.4" diameter="3.048" shape="octagon"/>
<wire x1="-2.54" y1="2.54" x2="10.16" y2="2.54" width="0.127" layer="21"/>
<wire x1="10.16" y1="2.54" x2="10.16" y2="-2.54" width="0.127" layer="21"/>
<wire x1="10.16" y1="-2.54" x2="10.16" y2="-7.62" width="0.127" layer="21"/>
<wire x1="10.16" y1="-7.62" x2="10.16" y2="-12.7" width="0.127" layer="21"/>
<wire x1="10.16" y1="-12.7" x2="10.16" y2="-17.78" width="0.127" layer="21"/>
<wire x1="10.16" y1="-17.78" x2="-2.54" y2="-17.78" width="0.127" layer="21"/>
<wire x1="-2.54" y1="-17.78" x2="-2.54" y2="2.54" width="0.127" layer="21"/>
<wire x1="10.16" y1="2.54" x2="20.32" y2="2.54" width="0.127" layer="21"/>
<wire x1="20.32" y1="2.54" x2="20.32" y2="-17.78" width="0.127" layer="21"/>
<wire x1="20.32" y1="-17.78" x2="10.16" y2="-17.78" width="0.127" layer="21"/>
<text x="19.05" y="-17.145" size="2.54" layer="21" rot="R90">&gt;NAME</text>
<text x="14.6812" y="2.0574" size="3.81" layer="21" rot="R180">+</text>
<text x="10.8204" y="-3.5052" size="3.81" layer="21" rot="R270">A</text>
<text x="14.7066" y="-11.8364" size="3.81" layer="21" rot="R90">B</text>
<wire x1="10.16" y1="-2.54" x2="15.24" y2="-2.54" width="0.127" layer="21"/>
<wire x1="10.16" y1="-7.62" x2="15.24" y2="-7.62" width="0.127" layer="21"/>
<wire x1="10.16" y1="-12.7" x2="15.24" y2="-12.7" width="0.127" layer="21"/>
<text x="14.986" y="-16.5608" size="5.08" layer="21" rot="R90">-</text>
</package>
</packages>
<symbols>
<symbol name="CONN4B">
<pin name="VCC" x="0" y="0" length="middle" direction="pwr" rot="R180"/>
<pin name="A" x="0" y="-5.08" length="middle" rot="R180"/>
<pin name="B" x="0" y="-10.16" length="middle" rot="R180"/>
<pin name="GND" x="0" y="-15.24" length="middle" direction="pwr" rot="R180"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="-17.78" width="0.254" layer="94"/>
<wire x1="-2.54" y1="-17.78" x2="-15.24" y2="-17.78" width="0.254" layer="94"/>
<wire x1="-15.24" y1="-17.78" x2="-15.24" y2="2.54" width="0.254" layer="94"/>
<wire x1="-15.24" y1="2.54" x2="-2.54" y2="2.54" width="0.254" layer="94"/>
<text x="-15.24" y="-20.32" size="1.778" layer="95">&gt;NAME</text>
</symbol>
</symbols>
<devicesets>
<deviceset name="CONN4B">
<gates>
<gate name="G$1" symbol="CONN4B" x="7.62" y="7.62"/>
</gates>
<devices>
<device name="" package="CONN4B">
<connects>
<connect gate="G$1" pin="A" pad="A"/>
<connect gate="G$1" pin="B" pad="B"/>
<connect gate="G$1" pin="GND" pad="GND"/>
<connect gate="G$1" pin="VCC" pad="VCC"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
<library name="switch">
<description>&lt;b&gt;Switches&lt;/b&gt;&lt;p&gt;
Marquardt, Siemens, C&amp;K, ITT, and others&lt;p&gt;
&lt;author&gt;Created by librarian@cadsoft.de&lt;/author&gt;</description>
<packages>
<package name="TL3XPO">
<description>&lt;b&gt;TINY SWITCH&lt;/b&gt;&lt;p&gt;
Source: http://www2.produktinfo.conrad.com/datenblaetter/700000-724999/705152-da-01-de-Subminiaturschalter_TL_36YO.pdf</description>
<wire x1="-5.08" y1="2.54" x2="5.08" y2="2.54" width="0.2032" layer="21"/>
<wire x1="5.08" y1="2.54" x2="5.08" y2="-2.54" width="0.2032" layer="21"/>
<wire x1="5.08" y1="-2.54" x2="-5.08" y2="-2.54" width="0.2032" layer="21"/>
<wire x1="-5.08" y1="-2.54" x2="-5.08" y2="2.54" width="0.2032" layer="21"/>
<wire x1="-3.175" y1="1.27" x2="0" y2="1.27" width="0.2032" layer="51"/>
<wire x1="-3.175" y1="-1.27" x2="0" y2="-1.27" width="0.2032" layer="51"/>
<wire x1="0" y1="1.27" x2="0" y2="-1.27" width="0.2032" layer="51" curve="-180"/>
<wire x1="-1.375" y1="1.275" x2="-1.35" y2="-1.3" width="0.2032" layer="51" curve="-273.242292"/>
<circle x="-3.175" y="0" radius="1.27" width="0.2032" layer="51"/>
<pad name="1" x="-2.54" y="0" drill="1.1" diameter="1.4224" shape="long" rot="R90"/>
<pad name="2" x="0" y="0" drill="1.1" diameter="1.4224" shape="long" rot="R90"/>
<pad name="3" x="2.54" y="0" drill="1.1" diameter="1.4224" shape="long" rot="R90"/>
<text x="-5.08" y="3.175" size="1.778" layer="25">&gt;NAME</text>
<text x="-5.08" y="-5.08" size="1.778" layer="27">&gt;VALUE</text>
</package>
</packages>
<symbols>
<symbol name="ON-MOM">
<wire x1="0" y1="-3.175" x2="0" y2="-2.54" width="0.254" layer="94"/>
<wire x1="0" y1="-2.54" x2="-1.524" y2="3.048" width="0.254" layer="94"/>
<wire x1="1.27" y1="2.54" x2="2.54" y2="2.54" width="0.254" layer="94"/>
<wire x1="2.54" y1="2.54" x2="2.54" y2="3.175" width="0.254" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-1.27" y2="2.54" width="0.254" layer="94"/>
<wire x1="-2.54" y1="2.54" x2="-2.54" y2="3.175" width="0.254" layer="94"/>
<wire x1="0.508" y1="1.27" x2="1.016" y2="1.524" width="0.1524" layer="94"/>
<wire x1="0.508" y1="1.27" x2="1.016" y2="1.016" width="0.1524" layer="94"/>
<wire x1="0.508" y1="1.27" x2="1.524" y2="1.27" width="0.1524" layer="94"/>
<text x="5.08" y="-2.54" size="1.778" layer="95" rot="R90">&gt;NAME</text>
<text x="7.62" y="-2.54" size="1.778" layer="96" rot="R90">&gt;VALUE</text>
<pin name="P" x="0" y="-5.08" visible="pad" length="short" direction="pas" rot="R90"/>
<pin name="S" x="2.54" y="5.08" visible="pad" length="short" direction="pas" rot="R270"/>
<pin name="O" x="-2.54" y="5.08" visible="pad" length="short" direction="pas" rot="R270"/>
</symbol>
</symbols>
<devicesets>
<deviceset name="TL32PO" prefix="S">
<description>&lt;b&gt;TINY SWITCH&lt;/b&gt; ON - MOM&lt;p&gt;
Source: http://www2.produktinfo.conrad.com/datenblaetter/700000-724999/705152-da-01-de-Subminiaturschalter_TL_36YO.pdf</description>
<gates>
<gate name="BEF1" symbol="ON-MOM" x="0" y="0"/>
</gates>
<devices>
<device name="" package="TL3XPO">
<connects>
<connect gate="BEF1" pin="O" pad="1"/>
<connect gate="BEF1" pin="P" pad="2"/>
<connect gate="BEF1" pin="S" pad="3"/>
</connects>
<technologies>
<technology name=""/>
</technologies>
</device>
</devices>
</deviceset>
</devicesets>
</library>
</libraries>
<attributes>
</attributes>
<variantdefs>
</variantdefs>
<classes>
<class number="0" name="default" width="0" drill="0">
</class>
</classes>
<parts>
<part name="LIMIT1" library="Mooov2" deviceset="CONN4B" device=""/>
<part name="ZMAX" library="switch" deviceset="TL32PO" device="" value="LimitSwitch"/>
<part name="ZMIN" library="switch" deviceset="TL32PO" device="" value="LimitSwitch"/>
<part name="LIMIT2" library="Mooov2" deviceset="CONN4B" device=""/>
<part name="XMIN1" library="switch" deviceset="TL32PO" device="" value="LimitSwitch"/>
<part name="XMAX1" library="switch" deviceset="TL32PO" device="" value="LimitSwitch"/>
<part name="YMIN2" library="switch" deviceset="TL32PO" device="" value="LimitSwitch"/>
<part name="YMAX2" library="switch" deviceset="TL32PO" device="" value="LimitSwitch"/>
</parts>
<sheets>
<sheet>
<plain>
<text x="60.96" y="55.88" size="1.778" layer="91">3 Axis : Separate Min and Max limit switches</text>
<text x="60.96" y="-7.62" size="1.778" layer="91">6 Axis : Combined Min and Max limit switches</text>
</plain>
<instances>
<instance part="LIMIT1" gate="G$1" x="106.68" y="48.26" rot="MR0"/>
<instance part="ZMAX" gate="BEF1" x="73.66" y="40.64" smashed="yes" rot="MR270">
<attribute name="NAME" x="78.74" y="45.72" size="1.778" layer="95" rot="MR0"/>
<attribute name="VALUE" x="88.9" y="35.56" size="1.778" layer="96" rot="MR0"/>
</instance>
<instance part="ZMIN" gate="BEF1" x="73.66" y="10.16" smashed="yes" rot="MR270">
<attribute name="NAME" x="78.74" y="15.24" size="1.778" layer="95" rot="MR0"/>
<attribute name="VALUE" x="88.9" y="5.08" size="1.778" layer="96" rot="MR0"/>
</instance>
<instance part="LIMIT2" gate="G$1" x="106.68" y="-12.7" rot="MR0"/>
<instance part="XMIN1" gate="BEF1" x="38.1" y="-20.32" smashed="yes" rot="MR270">
<attribute name="NAME" x="40.64" y="-15.24" size="1.778" layer="95" rot="MR0"/>
<attribute name="VALUE" x="53.34" y="-25.4" size="1.778" layer="96" rot="MR0"/>
</instance>
<instance part="XMAX1" gate="BEF1" x="73.66" y="-20.32" smashed="yes" rot="MR270">
<attribute name="NAME" x="78.74" y="-15.24" size="1.778" layer="95" rot="MR0"/>
<attribute name="VALUE" x="88.9" y="-25.4" size="1.778" layer="96" rot="MR0"/>
</instance>
<instance part="YMIN2" gate="BEF1" x="38.1" y="-50.8" smashed="yes" rot="MR270">
<attribute name="NAME" x="40.64" y="-45.72" size="1.778" layer="95" rot="MR0"/>
<attribute name="VALUE" x="53.34" y="-55.88" size="1.778" layer="96" rot="MR0"/>
</instance>
<instance part="YMAX2" gate="BEF1" x="73.66" y="-50.8" smashed="yes" rot="MR270">
<attribute name="NAME" x="78.74" y="-45.72" size="1.778" layer="95" rot="MR0"/>
<attribute name="VALUE" x="88.9" y="-55.88" size="1.778" layer="96" rot="MR0"/>
</instance>
</instances>
<busses>
</busses>
<nets>
<net name="GND" class="0">
<segment>
<pinref part="ZMAX" gate="BEF1" pin="S"/>
<wire x1="68.58" y1="38.1" x2="68.58" y2="33.02" width="0.1524" layer="91"/>
<pinref part="LIMIT1" gate="G$1" pin="GND"/>
<wire x1="68.58" y1="33.02" x2="106.68" y2="33.02" width="0.1524" layer="91"/>
<wire x1="68.58" y1="2.54" x2="106.68" y2="2.54" width="0.1524" layer="91"/>
<wire x1="106.68" y1="2.54" x2="106.68" y2="33.02" width="0.1524" layer="91"/>
<junction x="106.68" y="33.02"/>
<pinref part="ZMIN" gate="BEF1" pin="S"/>
<wire x1="68.58" y1="7.62" x2="68.58" y2="2.54" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$1" class="0">
<segment>
<pinref part="ZMAX" gate="BEF1" pin="P"/>
<wire x1="78.74" y1="40.64" x2="88.9" y2="40.64" width="0.1524" layer="91"/>
<wire x1="88.9" y1="40.64" x2="88.9" y2="43.18" width="0.1524" layer="91"/>
<pinref part="LIMIT1" gate="G$1" pin="A"/>
<wire x1="88.9" y1="43.18" x2="106.68" y2="43.18" width="0.1524" layer="91"/>
</segment>
</net>
<net name="VCC" class="0">
<segment>
<pinref part="LIMIT1" gate="G$1" pin="VCC"/>
<wire x1="106.68" y1="48.26" x2="96.52" y2="48.26" width="0.1524" layer="91"/>
<wire x1="96.52" y1="48.26" x2="68.58" y2="48.26" width="0.1524" layer="91"/>
<wire x1="96.52" y1="17.78" x2="96.52" y2="48.26" width="0.1524" layer="91"/>
<junction x="96.52" y="48.26"/>
<pinref part="ZMIN" gate="BEF1" pin="O"/>
<wire x1="96.52" y1="17.78" x2="68.58" y2="17.78" width="0.1524" layer="91"/>
<wire x1="68.58" y1="17.78" x2="68.58" y2="12.7" width="0.1524" layer="91"/>
<pinref part="ZMAX" gate="BEF1" pin="O"/>
<wire x1="68.58" y1="43.18" x2="68.58" y2="48.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$4" class="0">
<segment>
<pinref part="ZMIN" gate="BEF1" pin="P"/>
<wire x1="78.74" y1="10.16" x2="101.6" y2="10.16" width="0.1524" layer="91"/>
<wire x1="101.6" y1="10.16" x2="101.6" y2="38.1" width="0.1524" layer="91"/>
<pinref part="LIMIT1" gate="G$1" pin="B"/>
<wire x1="101.6" y1="38.1" x2="106.68" y2="38.1" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$5" class="0">
<segment>
<pinref part="XMIN1" gate="BEF1" pin="P"/>
<wire x1="43.18" y1="-20.32" x2="55.88" y2="-20.32" width="0.1524" layer="91"/>
<wire x1="55.88" y1="-20.32" x2="55.88" y2="-17.78" width="0.1524" layer="91"/>
<pinref part="XMAX1" gate="BEF1" pin="O"/>
<wire x1="55.88" y1="-17.78" x2="68.58" y2="-17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="GND1" class="0">
<segment>
<pinref part="XMAX1" gate="BEF1" pin="S"/>
<wire x1="68.58" y1="-22.86" x2="68.58" y2="-27.94" width="0.1524" layer="91"/>
<pinref part="LIMIT2" gate="G$1" pin="GND"/>
<wire x1="68.58" y1="-27.94" x2="106.68" y2="-27.94" width="0.1524" layer="91"/>
<pinref part="XMIN1" gate="BEF1" pin="S"/>
<wire x1="68.58" y1="-27.94" x2="33.02" y2="-27.94" width="0.1524" layer="91"/>
<wire x1="33.02" y1="-27.94" x2="33.02" y2="-22.86" width="0.1524" layer="91"/>
<junction x="68.58" y="-27.94"/>
<pinref part="YMIN2" gate="BEF1" pin="S"/>
<wire x1="33.02" y1="-53.34" x2="33.02" y2="-58.42" width="0.1524" layer="91"/>
<wire x1="33.02" y1="-58.42" x2="68.58" y2="-58.42" width="0.1524" layer="91"/>
<wire x1="68.58" y1="-58.42" x2="106.68" y2="-58.42" width="0.1524" layer="91"/>
<wire x1="106.68" y1="-58.42" x2="106.68" y2="-27.94" width="0.1524" layer="91"/>
<junction x="106.68" y="-27.94"/>
<pinref part="YMAX2" gate="BEF1" pin="S"/>
<wire x1="68.58" y1="-53.34" x2="68.58" y2="-58.42" width="0.1524" layer="91"/>
<junction x="68.58" y="-58.42"/>
</segment>
</net>
<net name="N$6" class="0">
<segment>
<pinref part="XMAX1" gate="BEF1" pin="P"/>
<wire x1="78.74" y1="-20.32" x2="88.9" y2="-20.32" width="0.1524" layer="91"/>
<wire x1="88.9" y1="-20.32" x2="88.9" y2="-17.78" width="0.1524" layer="91"/>
<pinref part="LIMIT2" gate="G$1" pin="A"/>
<wire x1="88.9" y1="-17.78" x2="106.68" y2="-17.78" width="0.1524" layer="91"/>
</segment>
</net>
<net name="VCC1" class="0">
<segment>
<pinref part="LIMIT2" gate="G$1" pin="VCC"/>
<pinref part="XMIN1" gate="BEF1" pin="O"/>
<wire x1="106.68" y1="-12.7" x2="96.52" y2="-12.7" width="0.1524" layer="91"/>
<wire x1="96.52" y1="-12.7" x2="33.02" y2="-12.7" width="0.1524" layer="91"/>
<wire x1="33.02" y1="-12.7" x2="33.02" y2="-17.78" width="0.1524" layer="91"/>
<pinref part="YMIN2" gate="BEF1" pin="O"/>
<wire x1="33.02" y1="-48.26" x2="33.02" y2="-43.18" width="0.1524" layer="91"/>
<wire x1="33.02" y1="-43.18" x2="96.52" y2="-43.18" width="0.1524" layer="91"/>
<wire x1="96.52" y1="-43.18" x2="96.52" y2="-12.7" width="0.1524" layer="91"/>
<junction x="96.52" y="-12.7"/>
</segment>
</net>
<net name="N$7" class="0">
<segment>
<pinref part="YMIN2" gate="BEF1" pin="P"/>
<wire x1="43.18" y1="-50.8" x2="55.88" y2="-50.8" width="0.1524" layer="91"/>
<wire x1="55.88" y1="-50.8" x2="55.88" y2="-48.26" width="0.1524" layer="91"/>
<pinref part="YMAX2" gate="BEF1" pin="O"/>
<wire x1="55.88" y1="-48.26" x2="68.58" y2="-48.26" width="0.1524" layer="91"/>
</segment>
</net>
<net name="N$8" class="0">
<segment>
<pinref part="YMAX2" gate="BEF1" pin="P"/>
<wire x1="78.74" y1="-50.8" x2="101.6" y2="-50.8" width="0.1524" layer="91"/>
<wire x1="101.6" y1="-50.8" x2="101.6" y2="-22.86" width="0.1524" layer="91"/>
<pinref part="LIMIT2" gate="G$1" pin="B"/>
<wire x1="101.6" y1="-22.86" x2="106.68" y2="-22.86" width="0.1524" layer="91"/>
</segment>
</net>
</nets>
</sheet>
</sheets>
</schematic>
</drawing>
</eagle>
