/*********************************************************************
本ソースリストおよびソフトウェアは、ライセンスフリーです。
利用、編集、再配布等が自由に行えますが、著作権表示の改変は禁止します。

                               Copyright (c) 2010-2015 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
設計事項
	元のcsvからデータを１時間ごと24時間分のデータ切り出し、
	平均化し、ファイル名(16文字まで)にYYMMDDを追加して保存する。
	指定日より前のファイルは変更しない。
	（使用例：処理する前日の日付を入力すると、前々日以前の再処理を防止できる）
	※指定日のファイルは実行されます。

                 0123 56 89
	入力 date = "2000/01/01";
	
	compress_data(
		char *date_lim , 				指定日
		char *filename , 				対象のファイル名（対象機器名）
		enum xbee_sensor_type val1, 	ファイル内のデータの種類 ※1
		enum xbee_sensor_type val2, 	
		enum xbee_sensor_type val3 ){	
		
	※1 : LIGHT,TEMP,HUMIDITY,WATT,BATT,PRESS,VALUE,TIMES,NA

CSVファイル形式

01234567890123456789012345678901234567890123456789012345678901234567890123456789

2012/8/11,19:06, 1008.511047, 263.533997, 28.250099
2012/8/11,19:06, 6286.179199, 263.533997, 28.250099
日付 17バイト    データ最大14バイト × 3          改行1バイト  60バイト(1データ最大)

時間: 1時間につき255データまで (データ記録は15秒に1回以下の頻度にする)
　　　24時間に1回しか呼ばない場合は1時間につき60データまで

*/
void _html_jump(char *html, char *filename){
	FILE *fp_html;
	char write[]    = "htdocs/000000_0123456789ABCDF.csv.html";
	
	if( strlen(filename) > 16) filename[16]='\0';
	sprintf(write, "htdocs/%s.html", filename );
	if( (fp_html = fopen(write , "w")) ){
		fprintf(fp_html, "<head><title>XBee Dairy Graph</title>\r\n");
		fprintf(fp_html, "<meta http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\r\n");
		fprintf(fp_html, "<meta http-equiv=\"REFRESH\" content=\"0; url=%s\">\r\n",html);
		fprintf(fp_html, "<meta name=\"viewport\" content=\"width=440, \">\r\n");
		fprintf(fp_html, "</head>\r\n");
		fprintf(fp_html, "<body>\r\n");
		fprintf(fp_html, "[<a href=\"%s\">ジャンプ</a>]　　[<a href=\"index.html\">戻る</a>]<br>\r\n",html);
		fprintf(fp_html, "</body></html>\r\n");
		fclose( fp_html );
	}
}

float _sensor_value_max(enum xbee_sensor_type val){	// 棒グラフのてっぺんとなる最大値を定義する
	switch( val ){									// 注意：次項目のofsを引いておく必要がある
		case LIGHT:
			return( 200. );
			break;
		case TEMP:
			return( 40. );
			break;
		case HUMIDITY:
			return( 20. );							// ofs=50%なので湿度は最大70% ここには70-50を入れる
			break;
		case WATT:
			return( 1000. );
			break;
		case BATT:
			return( 2. );							// ofs=2Vなので最大4Vになる。2～4Ｖ
			break;
		case PRESS:
			return( 26.5 );							// ofs=1000hPaなので最大1026.5hPa
			break;
		case VALUE:
			return( 10 );
			break;
		case TIMES:
			return( 10 );
			break;
		default:
			return( 100. );
			break;
	}
}

float _discrete_max_value(float in){
	if( in < 10 )			return 10;
	else if( in < 20 )		return 20;
	else if( in < 50 )		return 50;
	else if( in < 100 )		return 100;
	else if( in < 200 )		return 200;
	else if( in < 500 )		return 500;
	else if( in < 1000)		return 1000;
	else if( in < 2000)		return 2000;
	else if( in < 5000)		return 5000;
	else if( in < 10000)	return 10000;
	else if( in < 20000)	return 20000;
	else if( in < 50000)	return 50000;
	else if( in < 100000 )	return 100000;
	else if( in < 200000 )	return 200000;
	return(0);
}

float _sensor_value_ofs(enum xbee_sensor_type val){
	switch( val ){
		case LIGHT:
			return( 0. );
			break;
		case TEMP:
			return( 0. );
			break;
		case HUMIDITY:
			return( 50. );
			break;
		case WATT:
			return( 0. );
			break;
		case BATT:
			return( 2. );
			break;
		case PRESS:
			return( 1000. );
			break;
		case VALUE:
			return( 0. );
			break;
		case TIMES:
			return( 0 );
			break;
		default:
			return( 0. );
			break;
	}
}

int _day_end_month(int y,int m,int d){
	if(	/* 存在しない日 -1 を応答*/
		y < 2000 || m < 1 || d < 1 ||
		y > 2099 || m > 12 || d > 31 ||
		(( m == 4 || m == 6 || m == 9 || m == 11 ) && d > 30 )||
		(y%4 == 0 && m == 2 && d > 29) ||
		(y%4 != 0 && m == 2 && d > 28)
	) return(-1);
	else if(	/* 月末日 1 を応答*/
		( d == 31) ||
		(( m == 4 || m == 6 || m == 9 || m == 11 ) && d==30 )||
		(y%4 == 0 && m == 2 && d == 29) ||
		(y%4 != 0 && m == 2 && d == 28)
	) return(1);
	else return(0); /* 月末以外は 0 を応答 */
}

int _bar_graph_html(char *csv, float vday[][31], enum xbee_sensor_type *type ){
	//             0123456789012345678901234567890123456789
	// 入力 csv    data/120923_0123456789ABCDF.csv

	FILE *fp;
	//             0123456789012345678901234567890123456789
//	char html[] = "htdocs/120900_0123456789ABCDF.csv.html";	// 38文字+\0   39バイト
	char html[39]="htdocs/";
	char html_b[32]; // 先月リンク用のファイル名(htdocs/は入らない)
	char name[17];
	
	float vavr, vmax, vmin, min, max;
	int row,n;
	
	int i;
	int bar;
	int y,m,yb,mb;
	unsigned long ymd;		// y * 12 * 31 + m * 31 + d;

	html[7]='\0';
	strcat(html,&csv[5]);
	strcat(html,".html");
	html[11] = '0';
	html[12] = '0';
	strcpy(html_b,&html[7]);
	
	for( i=0 ; i<17 ; i++) name[i]= '\0';
	sscanf(csv, "data/%6ld_%[^.].csv", &ymd, name);
	y = 2000 + (int)(csv[5]-'0') * 10 + (int)(csv[6]-'0');
	m = (int)(csv[7]-'0') * 10 + (int)(csv[8]-'0');
	
	yb=y; mb=m;
	mb--;
	if(mb==0){
		mb=12;
		yb--;
	}
	html_b[0] = (char)((yb-2000)/10+'0');
	html_b[1] = (char)((yb%10)+'0');
	html_b[2] = (char)((mb/10)+'0');
	html_b[3] = (char)((mb%10)+'0');
	html_b[4] = '0';
	html_b[5] = '0';

	if( (fp = fopen(html , "w")) ){
		// printf("html_out(%ld)=[%s]\n", ymd, name );
		fprintf(fp, "<head><title>XBee monthly Graph</title>\r\n");
		fprintf(fp, "<meta http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\r\n");
		fprintf(fp, "<meta name=\"viewport\" content=\"width=440,  \">\r\n");
		fprintf(fp, "</head>\r\n");
		fprintf(fp, "<body>\r\n");
		fprintf(fp, "<center><h3>%d年%d月</h3></center><br><div align=right>[<a href=\"%s\">前月</a>]　　[<a href=\"index.html\">戻る</a>]<br></div><br>\r\n",y,m,html_b);
		fprintf(fp, "<h2>%s</h2>\r\n<center><table border=1>\r\n", name);
	
		for( row=0 ; row<3 ; row++){
			n=0;
			for(i=0;i<31;i++) if( vday[row][i] >= 0 ) n++;
			if(n>0){
				fprintf(fp, "<tr><td colspan=31><b>(%d) ",row+1);
				switch( type[row] ){
					case LIGHT:		fprintf(fp, "照度[Lux]");	break;
					case TEMP:		fprintf(fp, "温度[℃]");	break;
					case HUMIDITY:	fprintf(fp, "湿度[％]");	break;
					case WATT:		fprintf(fp, "電力[W]");		break;
					case BATT:		fprintf(fp, "電圧[V]");		break;
					case PRESS:		fprintf(fp, "気圧[hPa]");	break;
					case VALUE:		fprintf(fp, "値");			break;
					case TIMES:		fprintf(fp, "回数");		break;
					default:	break;
				}
				vavr=0.; vmax=0.; vmin=200000.; min=0.; n=0;
				for(i=0;i<31;i++){
					if(vday[row][i] >= 0){
						if( vmin > vday[row][i] ) vmin = vday[row][i];
						if( vmax < vday[row][i] ) vmax = vday[row][i];
						vavr += vday[row][i];
						n++;
					}
				}
				if(n>0) vavr /= (float)n; else vavr = 0.;
				if( vmin > vmax ) vmin = 0.;
				max=0.; 
				if(type[row]==VALUE ){
					min = 0.;
					max = _discrete_max_value(vmax);
					if( vmax / vmin < 1.1 && vmax > 20) {
						min = vmin; max = vmax;
					}
				}else{
					min = _sensor_value_ofs(type[row]);
					max = _sensor_value_max(type[row]) + min;
				}
				fprintf(fp, "</b><br>bottom=%d top=%d",(int)min,(int)max);
				fprintf(fp, " min=%d max=%d avr=%d</td></tr>\r\n<tr height=100>\r\n",(int)(vmin+0.5),(int)(vmax+0.5),(int)(vavr+0.5));
				for( i=0; i< 31 ; i++){
					if( vday[row][i] >= 0 ){
						bar = (int)( (vday[row][i]-min) / (max-min) * 100 );
						if( bar > 100 ) bar =100;
						if( bar < 0 ) bar =0;
						fprintf(fp, "<td valign=bottom height=100><img src=\"graph.gif\" height=%d width=10></td>\r\n",bar);
					}else{
						fprintf(fp, "<td height=100 width=10 bgcolor=\"lightgray\"></td>\r\n");
					}
				}
				fprintf(fp, "</tr>\r\n<tr>\r\n");
				for( i=0; i< 31 ; i++){
					if( _day_end_month(y,m,i+1)>=0 ) fprintf(fp, "<td align=center valign = top width=10><font size=1>%2d</font></td>\r\n",i+1);
					else fprintf(fp, "<td width=10 bgcolor=\"lightgray\"></td>\r\n");
				}
				fprintf(fp, "</tr>\r\n");
			}
		}
		fprintf(fp, "</table></center><br>\r\n");
		fprintf(fp, "[<a href=\"index.html\">戻る</a>]　　[<a href=\"%s\">前月</a>]<br>\r\n",html_b);
		fprintf(fp, "</body></html>\r\n");
		fclose( fp );
	}else{
		printf("ERROR to write : %s\n", html );
		return( -2 );
	}
	return(0);
}

/* 平均データavrをファイルに出力する。1日分のデータの書込み(元のCSVデータがあることを確認済みで呼ぶこと。) */
int _csv_daily_data(char *csv,int y,int m,int d,float *avr,enum xbee_sensor_type *type){
	FILE *fp;
	char s[128];	// 文字の空読み用
	unsigned long ymd;
	int yy,mm,dd;
	int i,j;
	float value[3];
	float vavr[3];
	float vday[3][31];

	int n[3];
	char name[17];
	char file_name[32];
	long fpoint;

	for(i=0;i<3;i++)for(j=0;j<31;j++)vday[i][j]=-1.;
	sscanf(csv,       "data/%6ld_%[^.].csv", &ymd, name);
	sprintf(file_name,"data/%s.csv", name);
	
	// データが既に書き込まれていないかどうかを確認する.ファイルがあればリターン
	if( (fp = fopen(file_name , "r")) ){		/* csvデータをリードでオープン(fp) */
		fseek(fp, 0L, SEEK_END);			// ファイルの最後から0バイトつまり最後へ移動
		fpoint = ftell(fp);					// 容量を保存
		if( fpoint > 127L ){
			fseek(fp, -160L, SEEK_END);
			fgets(s,127,fp);				// 1行(最大60バイトに少しマージン)の空読み
		}else{
			fseek(fp, 0L, SEEK_SET);		// ファイルの先頭へ
		}
		// 最終行を読んで、書き込み予定と同じ日や未来のデータが既にあればreturnする
		fscanf(fp, "%d/%d/%d, %f, %f, %f\n", &yy,&mm,&dd,&value[0],&value[1],&value[2]);
		if(yy >= y && mm >= m && dd >= d ){
			fclose(fp);
			return(-1);
		}
		// 最新の1か月分(31日)のデータを読み込む。64バイト×31 = 1984バイト
		if( ftell(fp) > 1984L ){
			fseek(fp, -1984L, SEEK_END);
			fgets(s,128,fp);				// 1行の空読み
		}else{
			fseek(fp, 0L, SEEK_SET);		// ファイルの先頭へ
		}
		// 書き込み日と同じ月の「異なる日を含む」データを読む
		while( !feof(fp) ){
			fscanf(fp, "%d/%d/%d, %f, %f, %f\n", &yy,&mm,&dd,&value[0],&value[1],&value[2]);
			if(yy==y && mm==m && dd>=1 && dd<=31){
				for(i=0;i<3;i++) vday[i][dd-1] = value[i];
				if(dd==d){
					fclose(fp);
					return(-2);
				}
			}
		}
		fclose(fp);
	}
	if( (fp = fopen(file_name , "a")) ){
		fprintf(fp, "%04d/%02d/%02d, %f, %f, %f\n", y, m, d, avr[0],avr[1],avr[2]);
		fclose(fp);
	}
	for(i=0;i<3;i++) vday[i][d-1] = avr[i];
	_bar_graph_html(csv, vday, type );
	if( _day_end_month(y,m,d)==1 ){
		d=0;
		if( (fp = fopen(file_name , "r")) ){		/* csvデータをリードでオープン(fp) */
			while( !feof(fp) ){
				fscanf(fp, "%d/%d/%d, %f, %f, %f\n", &yy,&mm,&dd,&value[0],&value[1],&value[2]);
				if(yy==y && mm==m ){
					d=-1;
					break;
				}
			}
			if(!d){
				fclose(fp);
				return(0);
			}
			for(i=0;i<3;i++){
				if(value[i] >= 0.){
					vavr[i]=value[i];
					n[i]=1;
				}else{
					vavr[i]=0.;
					n[i]=0;
				}
			}
			for(j=0;j<31;j++){
				fscanf(fp, "%d/%d/%d, %f, %f, %f\n", &yy,&mm,&dd,&value[0],&value[1],&value[2]);
				if(yy==y && mm==m ){
					for(i=0;i<3;i++){
						if(value[i] >= 0.){
							vavr[i] += value[i];
							n[i]++;
						}
					}
				}
				//if(feof(fp) == EOF ) break;
				if(feof(fp)) break;
			}
			fclose(fp);
			for(i=0;i<3;i++) if( n[i] > 0 ) vavr[i] /= (float)n[i]; else vavr[i] = -1;
			
			/*  ファイル名の変更 1データ形式 */
			sscanf(csv,       "data/%6ld_%[^.].csv", &ymd, name);
			sprintf(file_name,"data/%s.mon.csv", name);
			
			if( (fp = fopen(file_name , "a")) ){
				fprintf(fp, "%04d/%02d/01, %f, %f, %f\n", y, m, vavr[0],vavr[1],vavr[2]);
				fclose(fp);
			}
		}
	}
	return(0);
}

int _delete_csv_daily_data(char *csv){
// dataフォルダ内のcsvデータを消す ※実行注意
	FILE *fp;
	char file_name[32];
	char name[17];
	
	sprintf(file_name,"data/%s", csv);
	if( (fp = fopen(file_name,"w")) ){
		fclose(fp);
	}else return(-1);
	
	sscanf(csv, "%[^.].csv", name);
	sprintf(file_name,"data/%s.mon.csv", name);
	if( (fp = fopen(file_name,"w")) ){
		fclose(fp);
	}else return(-1);
	return(0);
}

int csv2html( char *csv, char *html, int valn, char *html_b, enum xbee_sensor_type *type){
	// enum LIGHT,TEMP,HUMIDITY,WATT,BATT,PRESS,VALUE,TIMES,NA

	FILE *fp, *fp_html;
	
	float value[3][24];	// 平均計算用：各入力値
	float min,max;				// 最大値
	float vmax, vmin;
	float vavr[3];
	int flag24=1;				// データが24時間分揃っているかどうか
	
	char name[17];					//  0123456
	char html_m[32];			// 当月 120923_ABC.csv.html

	int i,j,row,n;
	int bar;
	int y, m, d, hh, mm;
	unsigned long ymd;
	
	for( i=0 ; i<17 ; i++) name[i]= '\0'; 
	sscanf(csv, "data/%6ld_%[^.].csv", &ymd, name);
	
	strcpy(html_m,html_b);
	html_m[4] = '0';
	html_m[5] = '0';
	
	for(i=0;i<3;i++)for(j=0;j<24;j++)value[i][j]=-1.;
	
	/* html 出力 */
	if( (fp = fopen(csv , "r")) ){		/* csvデータをリードでオープン(fp) */
		for( i=0; i< 24 ; i++){
			if( valn == 3 ){
				if( fscanf(fp, "%d/%d/%d,%d:%d,%f,%f,%f\n", &y,&m,&d,&hh,&mm,&value[0][i], &value[1][i], &value[2][i] ) == EOF ){
					flag24=0;
				}
			}else if( valn == 2 ){
				if( fscanf(fp, "%d/%d/%d,%d:%d,%f,%f\n", &y,&m,&d,&hh,&mm,&value[0][i], &value[1][i] )== EOF ){
					flag24=0;
				}
			}else if( valn == 1 ){
				if( fscanf(fp, "%d/%d/%d,%d:%d,%f\n", &y,&m,&d,&hh,&mm,&value[0][i] )== EOF ){
					flag24=0;
				}
			}
			if(!flag24){	// EOFとなった時 ※これでも最終行は読めるはず。
				value[0][i] = -1.;
				value[1][i] = -1.;
				value[2][i] = -1.;
				break;
			}
		}
		fclose( fp );
		
		if( (fp_html = fopen(html , "w")) ){
			// printf("html_out(%ld)=[%s]\n", ymd, name );
			fprintf(fp_html, "<head><title>XBee Daily Graph</title>\r\n");
			fprintf(fp_html, "<meta http-equiv=\"Content-type\" content=\"text/html; charset=Shift_JIS\">\r\n");
			fprintf(fp_html, "<meta name=\"viewport\" content=\"width=440,  \">\r\n");
			fprintf(fp_html, "</head>\r\n");
			fprintf(fp_html, "<body>\r\n");
			fprintf(fp_html, "<center><h3>%d年%d月%d日</h3></center><br><div align=right>[<a href=\"%s\">前日</a>]　　[<a href=\"%s\">月間</a>]　　[<a href=\"index.html\">戻る</a>]<br></div><br>\r\n",y,m,d,html_b,html_m);
			fprintf(fp_html, "<h2>%s</h2>\r\n<center><table border=1>\r\n", name);
			
			for( row = 0 ; row < valn ; row++){
				fprintf(fp_html, "<tr><td colspan=24><b>(%d) ",row+1);
				switch( type[row] ){
					case LIGHT:		fprintf(fp_html, "照度[Lux]");	break;
					case TEMP:		fprintf(fp_html, "温度[℃]");	break;
					case HUMIDITY:	fprintf(fp_html, "湿度[％]");	break;
					case WATT:		fprintf(fp_html, "電力[W]");	break;
					case BATT:		fprintf(fp_html, "電圧[V]");	break;
					case PRESS:		fprintf(fp_html, "気圧[hPa]");	break;
					case VALUE:		fprintf(fp_html, "値");			break;
					case TIMES:		fprintf(fp_html, "回数");		break;
					default:	break;
				}
				vmax = 0.; vmin = 99999.; vavr[row]= 0.; n=0;
				for( i=0; i< 24 ; i++){
					if( value[row][i] >= 0 ){
						if( vmin > value[row][i] && value[row][i] >= 0 ) vmin = value[row][i];
						if( vmax < value[row][i] && value[row][i] >= 0 ) vmax = value[row][i];
						vavr[row] += value[row][i];
						n++;
					}
				}
				if( n > 0 ) vavr[row] /= (float)n; else vavr[row] = 0.;
				if( vmin > vmax ) vmin = 0.;
				if( type[row] == VALUE ){
					min=0.;
					max = _discrete_max_value(vmax);
					if( vmax / vmin < 1.1 && vmax > 20) {
						min = vmin; max = vmax;
					}
				}else{
					min = _sensor_value_ofs(type[row]);
					max = _sensor_value_max(type[row]) + min;
				}
				fprintf(fp_html, "</b><br>bottom=%d top=%d",(int)min,(int)max);
				fprintf(fp_html, " min=%d max=%d avr=%d</td></tr>\r\n<tr height=100>\r\n",(int)(vmin+0.5),(int)(vmax+0.5),(int)(vavr[row]+0.5));
				for( i=0; i< 24 ; i++){
					if( value[row][i] >= 0 ){
						bar = (int)( (value[row][i]-min) / (max-min) * 100 );
						if( bar > 100 ) bar =100;
						if( bar < 0 ) bar =0;
						fprintf(fp_html, "<td valign=bottom height=100><img src=\"graph.gif\" height=%d width=12></td>\r\n",bar);
					}else{
						fprintf(fp_html, "<td valign=bottom height=100 width=12 bgcolor=\"lightgray\"></td>\r\n");
					}
				}
				fprintf(fp_html, "</tr>\r\n<tr>\r\n");
				for( i=0; i< 24 ; i++) fprintf(fp_html, "<td align=center valign = top width=12><font size=1>%2d</font></td>\r\n",i);
				fprintf(fp_html, "</tr>\r\n");
			}
			fprintf(fp_html, "</table></center><br>\r\n");
			fprintf(fp_html, "[<a href=\"index.html\">戻る</a>]　　[<a href=\"%s\">月間</a>]　　[<a href=\"%s\">前日</a>]<br>\r\n",html_m,html_b);
			fprintf(fp_html, "</body></html>\r\n");
			fclose( fp_html );
		}else{
			printf("ERROR to write : %s\n", html );
			return( -2 );
		}
	}else{
		printf("ERROR open for read :%s\n",csv);
		return( -1 );
	}
	if(flag24) _csv_daily_data(csv,y,m,d,vavr,type);
	return(0);
}

int compress_data(char *date_lim , char *filename , enum xbee_sensor_type val1, enum xbee_sensor_type val2, enum xbee_sensor_type val3 ){
	FILE *fpr,*fpw,*fpe;
	
	// enum xbee_sensor_type LIGHT,TEMP,HUMIDITY,WATT,BATT,PRESS,VALUE,TIMES,NA
	
	//                 012345Y7M9D12345678901234567890123456789
	char write[]    = "data/120923_0123456789ABCDF.csv";		// 31文字 32バイト
	char html[]     = "htdocs/120923_0123456789ABCDF.csv.html";	// 38文字+\0   39バイト
	char html_b[]   = "120923_0123456789ABCDF.csv.html";	// その前の分 31文字   32バイト
	char html_b2[]  = "120923_0123456789ABCDF.csv.html";	// 現在分
	
	int y,m,d;		// 次回の年月日
	unsigned long ymd;		// y * 12 * 31 + m * 31 + d;
	int yb,mb,db;	// 前回の年月日
	int yl,ml,dl;	// 入力されて date_limの展開値
	unsigned long ymd_l;		// y * 12 * 31 + m * 31 + d;
	int hh,hour;	// 次回の時、今の時
	int hhb;		// 前回の時
	int mm;
	int vals;		// 0～3 何列のデータがあるか
	
	char s[128];	// 文字処理用
	float value[3][256];	// 平均計算用：各入力値
	float sum;				// 平均計算用：合計値
	int div;				// 平均計算用：除算値
	int i,j;				// 汎用変数
	byte f;					// 汎用フラグ
	int valn = 3;
	long fpoint;

	enum xbee_sensor_type type[3];
	type[0] = val1;
	type[1] = val2;
	type[2] = val3;
	
	if( val3 == NA ) valn = 2;
	if( val2 == NA ) valn = 1;
	if( val1 == NA ) return (-1);

	// if( valn < 1 || valn > 3 ) return (-1);
	yl=0;
	for( i=0; i<4 ; i++ ){
		yl *= 10;
		yl += (int)date_lim[i] - (int)'0';
	}
	ml=0;
	for( i=5; i<7 ; i++ ){
		ml *= 10;
		ml += (int)date_lim[i] - (int)'0';
	}
	dl=0;
	for( i=8; i<10 ; i++ ){
		dl *= 10;
		dl += (int)date_lim[i] - (int)'0';
	}
	ymd_l = (unsigned long)yl * 12 * 31 + (unsigned long)ml * 31 + (unsigned long)dl;

	printf("-- compress started (%s)\n", filename);
	if( (fpe = fopen("data/log.txt","a")) ){
		fprintf(fpe,
			"-----------------------------------------------------------------\ncompress started date=%s file=%s valn=%d\n",
			date_lim,
			filename,
			valn
		);
		if( strlen(filename) > 16){
			fprintf(fpe,"ERROR filename:%s",filename);
			filename[16]='\0';
			fprintf(fpe,"-> %s\n",filename);
		}
		if( (fpr = fopen(filename , "r")) ){
			/* データ検索 */
			fseek(fpr, 0L, SEEK_END);		// ファイルの最後から0バイトつまり最後へ移動
			fpoint = ftell(fpr);			// その位置を取得。つまりファイルサイズ
			fpoint /= 2;					// ファイルサイズの半分を代入
			if( strcmp(date_lim,"2000/01/01" ) ){	// 2000/01/01と一致し無い時（通常動作時）
				if( fpoint <= 172800L ) fseek(fpr, 0L, SEEK_SET);		// ファイルの先頭に戻る
				else do{	// 172800バイト以上の時 60データ×24時間×60バイト×2 = 172800 (256データ×60×2=30720)
					fseek(fpr, -fpoint, SEEK_END);
					fgets(s,127,fpr);		// 1行の空読み
					fscanf(fpr,"%d/%d/%d,%d:%d,",&y,&m,&d,&hh,&mm);
					fgets(s,127,fpr);		// 1行の空読み
				//	printf("fp=%ld,y=%d,m=%d,d=%d\n",fpoint,y,m,d);	//デバッグ用
					ymd = (unsigned long)y * 12 * 31 + (unsigned long)m * 31 + (unsigned long)d;
					if( ymd >= ymd_l ){
						fseek(fpr, -fpoint, SEEK_CUR);
						fpoint=0;
					}else{
						fpoint /= 2;
					}
				}while( fpoint > 172800L );
			}else{
				fseek(fpr, 0L, SEEK_SET);		// ファイルの先頭に戻る
				if( _delete_csv_daily_data(filename) == 0){
					printf("### delete file: data/%s\n",filename);
					fprintf(fpe,"delete file: data/%s\n",filename);
				}else{
					fprintf(fpe,"ERROR cannot delete file:data/%s\n",filename);
				}
			}
			/* データ読み取り処理 */
			yb=0; mb=0; db=0; hhb=0; ymd=0UL;
			y=0;  m=0;  d= 0; hh=0;
			sprintf( html_b, "index.html");
			sprintf( html_b2, "index.html");
		//	printf("data read\n");	//デバッグ用
			do{	// 読取１ファイルの繰り返し
				/* 対象日のデータを探す */
				if( db == 0 ){	// ループの初回
					while( ymd_l > ymd ){
						yb=y;	mb=m;	db=d;	hhb=hh;	//読み取り前のバックアップ
						if( fscanf(fpr,"%d/%d/%d,%d:%d,",&y,&m,&d,&hh,&mm) != EOF ){
						//	printf("ymd:%04d/%02d/%02d,%02d:%02d (%ld>%ld)\n", y, m, d, hh, mm, ymd_l, ymd);	// test	//デバッグ用
						//	fprintf(fpe,"ymd:%04d/%02d/%02d,%02d:%02d (%ld>%ld)\n", y, m, d, hh, mm, ymd_l, ymd);	// test	//デバッグ用
							ymd = (unsigned long)y * 12 * 31 + (unsigned long)m * 31 + (unsigned long)d;
							if( y < 2000 || y > 2099 || m < 1 || m > 12 || d < 1 || d > 31 || hh < 0 || hh > 23){
								fprintf(fpe,"ERROR ymd:%04d/%02d/%02d,%02d:00\n", y, m, d, hh);
								printf("ERROR ymd\n");
								fclose( fpr );
								fclose( fpe );
								return(-1);
							}
							if( ymd_l > ymd ){
								// fprintf(fpe, "ymd skipped %04d/%02d/%02d,%02d:00\n", y, m, d, hh); // test
								fscanf(fpr,"%[^\r\n]\n", s);
								// fprintf(fpe, "<%s>\n", s); // test
								
								/*
								データの無い期間が日をまたぐと先の日のデータが入ってしまう
								6/22の4時～6/24の11時までデータが欠落した場合：
								
								averaging  data/120622_press.csv
								2012/06/22,00:00, 23.810288, 994.871094
								2012/06/22,01:00, 23.658817, 993.417969
								2012/06/22,02:00, 23.517646, 993.701057
								2012/06/22,03:00, 23.439710, 993.985983
								2012/06/22,04:00, 23.637040, 994.657046
								2012/06/24,05:00, -1.0, -1.0
								2012/06/24,06:00, -1.0, -1.0
								2012/06/24,07:00, -1.0, -1.0
								2012/06/24,08:00, -1.0, -1.0
								2012/06/24,09:00, -1.0, -1.0
								2012/06/24,10:00, -1.0, -1.0
								2012/06/24,11:00, 24.618030, 999.320377
								2012/06/24,12:00, 25.611770, 999.379366
								2012/06/24,13:00, 24.757775, 997.925955
								*/
							}
							if( db != d && yb > 2000){
								sprintf( html_b, "%s" , html_b2 );
								sprintf( html_b2, "%02d%02d%02d_%s.html", yb-2000, mb, db , filename );
							}
						}else{
						//	printf("done (ymd)\n");	//デバッグ用
							fprintf(fpe,"done (nothing to do)\n");
							printf("done (nothing to do)\n");
							fclose( fpr );
							fclose( fpe );
							return( 0 );
						}
					}
				}
				
				/* 書き込み １日分*/
				/* csv 出力 */
			//	printf("data write\n");	//デバッグ用
				sprintf(write, "data/%02d%02d%02d_%s", y-2000, m, d , filename );
				fprintf(fpe,"averaging  %s\n", write );
				if( (fpw = fopen(write , "w")) ){
					f = 1;	// 同じ日の処理を行っていることを示すフラグ
					for( hour=0 ; hour < 24 ; hour++ ){	// 同じ日内の24時間分の繰り返し
						while( hour < hh ){
							fprintf(fpw, "%04d/%02d/%02d,%02d:00", y, m, d, hour);
							fprintf(fpe, "skipped %04d/%02d/%02d,%02d:00\n", y, m, d, hour);
							for( j = 0 ; j < valn ; j++) fprintf(fpw, ", -1.0");
							fprintf(fpw, "\n" );
							if( hour >= 23 ) break;
							hour++;
						}
						vals= 1;
						for( i=0 ; i<256 ; i++ ){
							/* データ読み取り */
							fscanf(fpr,"%[^\r\n]\n",s);
							if( valn == 3 ){
								sscanf(s, "%f,%f,%f", &value[0][i], &value[1][i], &value[2][i] );
							}else if( valn == 2 ){
								sscanf(s, "%f,%f", &value[0][i], &value[1][i] );
							}else if( valn == 1 ){
								sscanf(s, "%f", &value[0][i] );
							}
							/* test*/
						//	printf("test[%d,%ld] %s %f\n",i,ymd,s,value[0][i]);
							
							/* 次回の時刻の読み取り */
							yb=y;	mb=m;	db=d;	hhb=hh;	//読み取り前のバックアップ
							if( fscanf(fpr,"%d/%d/%d,%d:%d,",&y,&m,&d,&hh,&mm) != EOF ){
								vals=i+1;
								// printf("ymd:%04d/%02d/%02d,%02d:%02d\n", y, m, d, hh, mm);	// test
								// fprintf(fpe,"ymd:%04d/%02d/%02d,%02d:%02d\n", y, m, d, hh, mm);	// test
								ymd = (unsigned long)y * 12 * 31 + (unsigned long)m * 31 + (unsigned long)d;
								if( y < 2000 || y > 2099 || m < 1 || m > 12 || d < 1 || d > 31 || hh < 0 || hh > 23){
									fprintf(fpe,"ERROR ymd:%04d/%02d/%02d,%02d:00\n", y, m, d, hour);
									printf("ERROR ymd\n");
									fclose( fpw );
									fclose( fpr );
									fclose( fpe );
									return(-1);
								}
								/* 日付が一致していない場合にhtml用の日付を更新
									html_b		b2のバックアップ
									html_b2		現在のファイル名(不一致になる前の値)に更新
								*/
								if( db != d && yb > 2000 ){
									sprintf( html_b, "%s" , html_b2 );
									sprintf( html_b2, "%02d%02d%02d_%s.html", yb-2000, mb, db , filename );
								}
								/* 読取中の日付が一致していない場合にhourループを抜ける */
								if( yb != y || mb != m || db != d ){
									i = 256;		// iの値はvalsにi+1がバックアップされている
									// hour = 24;	// 処理中の時刻が変わってしまうのでhourは変更できない。
									f = 0;			// hourループを抜けるためのフラグ操作
									break;
								}
								/* 読取中のhour時と一致していない場合にｆｏｒ（ｉ）ループを抜ける */
								if( hour != hh){
									i = 256;		// iの値はvalsにi+1がバックアップされている
									break;
								}
							} else {
								/* 読み込みファイルのＥＯＦにて変換終了処理 */
							//	vals=i;
								/* CSVデータ出力(最後の時刻分) hour = 23 のとき、もしくは中断されていたら 23以下もあり得る */
								fprintf(fpw, "%04d/%02d/%02d,%02d:00", yb, mb, db, hour);
								for( j = 0 ; j < valn ; j++){
									fprintf(fpw, ", ");
									div = 0;
									sum = 0.;
									for( i = 0 ; i < vals ; i++ ){
										if( value[j][i] >= 0.0 ){
											if(	(j == 0 && val1 == TIMES) ||	// データが「回数の時」
												(j == 1 && val2 == TIMES) ||
												(j == 2 && val3 == TIMES) ){
												sum += 1;
												div = 1;
											}else{							// データが回数以外の時
												sum += value[j][i];
												div++;
											}
										}
									}
									// fprintf(fpe,"%s j=%d sum=%f div=%f\n", write, j ,sum, (float)div); //test
									if( div > 0 ) fprintf(fpw, "%f", ( sum / (float)div ) );
									else{
										fprintf(fpw, "-1.0" );		// div=0の時、そうなるのはvalueが全て-1の時
										fprintf(fpe, "WARNING:vals=%d\n",vals);
									}
								}
								fprintf(fpw, "\n" );
								/* CSV出力ここまで */
								
								fclose( fpw );
								fclose( fpr );
								/* html出力 */
								sprintf( html, "htdocs/%02d%02d%02d_%s.html", yb-2000, mb, db , filename );
								fprintf(fpe,"html_out %s\n", html );
								
								/*		EOFで終わった場合は、html_b2の日付が更新されていないので、
										前々日はhtml_bではなくhtml_b2になる。
										このため、 csv2htmlで「前日」のURL指定はhtml_b2を使っている。 */
								i = csv2html( write, html, valn , html_b2, type);
								if( i == -1 ) fprintf(fpe,"ERROR open for read :%s\n",write);
								if( i == -2 ) fprintf(fpe,"ERROR to write : %s\n", html );
								/* return 処理 */
								fprintf(fpe,"done (EOF)\n");
								printf("done (EOF)\n");
								fclose( fpe );
								sprintf( html_b2, "%02d%02d%02d_%s.html", yb-2000, mb, db , filename );  // html_b2をCSVデータ出力(fpw)の日付に
								_html_jump( html_b2, filename);
								return( 0 );
							}
						}
						if( vals > 256) vals = 256;	// for(i)で256回を呼んでもデータが残っている時は捨てる
						
						/* CSVデータ出力(毎時１回) */
						fprintf(fpw, "%04d/%02d/%02d,%02d:00", yb, mb, db, hour);
						for( j = 0 ; j < valn ; j++){
							fprintf(fpw, ", ");
							div = 0;
							sum = 0.;
							for( i = 0 ; i < vals ; i++ ){
								if( value[j][i] >= 0.0 ){
									if(	(j == 0 && val1 == TIMES) ||	// データが「回数の時」
										(j == 1 && val2 == TIMES) ||
										(j == 2 && val3 == TIMES) ){
										sum += 1;
										div = 1;
									}else{							// データが回数以外の時
										sum += value[j][i];
										div++;
									}
								}
							}
							// fprintf(fpe,"%s j=%d sum=%f div=%f\n", write, j ,sum, (float)div); //test
							if( div > 0 ) fprintf(fpw, "%f", ( sum / (float)div ) );
							else{
								fprintf(fpw, "-1.0" );		// div=0の時、そうなるのはvalueが全て-1の時
								fprintf(fpe, "WARNING:vals=%d\n",vals);
							}
						}
						fprintf(fpw, "\n" );
						/* バッファ256個を超えた場合の読み捨て処理 */
						if( vals == 256 ){
							fprintf(fpe, "WARNING:vals=%d\n",vals);
							do{
								fscanf(fpr,"%[^\r\n]\n", s);
								yb=y;	mb=m;	db=d;	hhb=hh;	//読み取り前のバックアップ
								if( fscanf(fpr,"%d/%d/%d,%d:%d,",&y,&m,&d,&hh,&mm) != EOF ){
									if( yb != y || mb != m || db != d ) hhb = -1;
								}else hhb = -1;
							}while( hh == hhb);		// 同じ時間のものは読み捨てる
						}
						/* 日付が変わっていた時のbreak処理 */
						if( f == 0 ) break;
					}
					fclose( fpw );
					fprintf(fpe,"exported   %s\n", write );
				}else{
					fprintf(fpe,"ERROR open for write :%s\n",write);
					break;
				}
				// fprintf(fpe, "WARNING:unwanted data existed befor EOF\n");
				/* html出力 。
				日付の変わり目でhtml出力する部分。
				日々のバッチ処理では、日の変わり目の後にデータが無いので通らない。
				初回のバッチ処理では、全件変換なので、日が変わるたびに通る */
				sprintf( html, "htdocs/%02d%02d%02d_%s.html", yb-2000, mb, db , filename );
				fprintf(fpe,"html_out %s\n", html );
				
				/*		23時までいかなかった場合は、？？？
				*/
				i=csv2html( write, html, valn , html_b, type);
				if( i == -1 ) fprintf(fpe,"ERROR open for read :%s\n",write);
				if( i == -2 ) fprintf(fpe,"ERROR to write : %s\n", html );
		//	}while( feof(fpr) != EOF );
			}while( feof(fpr) == 0 );
			fclose( fpr );
			sprintf( html_b2, "%02d%02d%02d_%s.html", yb-2000, mb, db , filename );  // html_b2をCSVデータ出力(fpw)の日付に
			_html_jump( html_b2, filename);
		}else{
			fprintf(fpe,"ERROR open for log:%s\n",filename);
		}
		fprintf(fpe,"done (END)\n");
		fclose( fpe );
	}
	printf("done (END)\n");
	return(0);
}
