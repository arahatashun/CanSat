/*********************************************************************
�{�\�[�X���X�g����у\�t�g�E�F�A�́A���C�Z���X�t���[�ł��B
���p�A�ҏW�A�Ĕz�z�������R�ɍs���܂����A���쌠�\���̉��ς͋֎~���܂��B

                               Copyright (c) 2010-2015 Wataru KUNINO
                               http://www.geocities.jp/bokunimowakaru/
*********************************************************************/
/*
�݌v����
	����csv����f�[�^���P���Ԃ���24���ԕ��̃f�[�^�؂�o���A
	���ω����A�t�@�C����(16�����܂�)��YYMMDD��ǉ����ĕۑ�����B
	�w������O�̃t�@�C���͕ύX���Ȃ��B
	�i�g�p��F��������O���̓��t����͂���ƁA�O�X���ȑO�̍ď�����h�~�ł���j
	���w����̃t�@�C���͎��s����܂��B

                 0123 56 89
	���� date = "2000/01/01";
	
	compress_data(
		char *date_lim , 				�w���
		char *filename , 				�Ώۂ̃t�@�C�����i�Ώۋ@�햼�j
		enum xbee_sensor_type val1, 	�t�@�C�����̃f�[�^�̎�� ��1
		enum xbee_sensor_type val2, 	
		enum xbee_sensor_type val3 ){	
		
	��1 : LIGHT,TEMP,HUMIDITY,WATT,BATT,PRESS,VALUE,TIMES,NA

CSV�t�@�C���`��

01234567890123456789012345678901234567890123456789012345678901234567890123456789

2012/8/11,19:06, 1008.511047, 263.533997, 28.250099
2012/8/11,19:06, 6286.179199, 263.533997, 28.250099
���t 17�o�C�g    �f�[�^�ő�14�o�C�g �~ 3          ���s1�o�C�g  60�o�C�g(1�f�[�^�ő�)

����: 1���Ԃɂ�255�f�[�^�܂� (�f�[�^�L�^��15�b��1��ȉ��̕p�x�ɂ���)
�@�@�@24���Ԃ�1�񂵂��Ă΂Ȃ��ꍇ��1���Ԃɂ�60�f�[�^�܂�

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
		fprintf(fp_html, "[<a href=\"%s\">�W�����v</a>]�@�@[<a href=\"index.html\">�߂�</a>]<br>\r\n",html);
		fprintf(fp_html, "</body></html>\r\n");
		fclose( fp_html );
	}
}

float _sensor_value_max(enum xbee_sensor_type val){	// �_�O���t�̂Ă��؂�ƂȂ�ő�l���`����
	switch( val ){									// ���ӁF�����ڂ�ofs�������Ă����K�v������
		case LIGHT:
			return( 200. );
			break;
		case TEMP:
			return( 40. );
			break;
		case HUMIDITY:
			return( 20. );							// ofs=50%�Ȃ̂Ŏ��x�͍ő�70% �����ɂ�70-50������
			break;
		case WATT:
			return( 1000. );
			break;
		case BATT:
			return( 2. );							// ofs=2V�Ȃ̂ōő�4V�ɂȂ�B2�`4�u
			break;
		case PRESS:
			return( 26.5 );							// ofs=1000hPa�Ȃ̂ōő�1026.5hPa
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
	if(	/* ���݂��Ȃ��� -1 ������*/
		y < 2000 || m < 1 || d < 1 ||
		y > 2099 || m > 12 || d > 31 ||
		(( m == 4 || m == 6 || m == 9 || m == 11 ) && d > 30 )||
		(y%4 == 0 && m == 2 && d > 29) ||
		(y%4 != 0 && m == 2 && d > 28)
	) return(-1);
	else if(	/* ������ 1 ������*/
		( d == 31) ||
		(( m == 4 || m == 6 || m == 9 || m == 11 ) && d==30 )||
		(y%4 == 0 && m == 2 && d == 29) ||
		(y%4 != 0 && m == 2 && d == 28)
	) return(1);
	else return(0); /* �����ȊO�� 0 ������ */
}

int _bar_graph_html(char *csv, float vday[][31], enum xbee_sensor_type *type ){
	//             0123456789012345678901234567890123456789
	// ���� csv    data/120923_0123456789ABCDF.csv

	FILE *fp;
	//             0123456789012345678901234567890123456789
//	char html[] = "htdocs/120900_0123456789ABCDF.csv.html";	// 38����+\0   39�o�C�g
	char html[39]="htdocs/";
	char html_b[32]; // �挎�����N�p�̃t�@�C����(htdocs/�͓���Ȃ�)
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
		fprintf(fp, "<center><h3>%d�N%d��</h3></center><br><div align=right>[<a href=\"%s\">�O��</a>]�@�@[<a href=\"index.html\">�߂�</a>]<br></div><br>\r\n",y,m,html_b);
		fprintf(fp, "<h2>%s</h2>\r\n<center><table border=1>\r\n", name);
	
		for( row=0 ; row<3 ; row++){
			n=0;
			for(i=0;i<31;i++) if( vday[row][i] >= 0 ) n++;
			if(n>0){
				fprintf(fp, "<tr><td colspan=31><b>(%d) ",row+1);
				switch( type[row] ){
					case LIGHT:		fprintf(fp, "�Ɠx[Lux]");	break;
					case TEMP:		fprintf(fp, "���x[��]");	break;
					case HUMIDITY:	fprintf(fp, "���x[��]");	break;
					case WATT:		fprintf(fp, "�d��[W]");		break;
					case BATT:		fprintf(fp, "�d��[V]");		break;
					case PRESS:		fprintf(fp, "�C��[hPa]");	break;
					case VALUE:		fprintf(fp, "�l");			break;
					case TIMES:		fprintf(fp, "��");		break;
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
		fprintf(fp, "[<a href=\"index.html\">�߂�</a>]�@�@[<a href=\"%s\">�O��</a>]<br>\r\n",html_b);
		fprintf(fp, "</body></html>\r\n");
		fclose( fp );
	}else{
		printf("ERROR to write : %s\n", html );
		return( -2 );
	}
	return(0);
}

/* ���σf�[�^avr���t�@�C���ɏo�͂���B1�����̃f�[�^�̏�����(����CSV�f�[�^�����邱�Ƃ��m�F�ς݂ŌĂԂ��ƁB) */
int _csv_daily_data(char *csv,int y,int m,int d,float *avr,enum xbee_sensor_type *type){
	FILE *fp;
	char s[128];	// �����̋�ǂݗp
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
	
	// �f�[�^�����ɏ������܂�Ă��Ȃ����ǂ������m�F����.�t�@�C��������΃��^�[��
	if( (fp = fopen(file_name , "r")) ){		/* csv�f�[�^�����[�h�ŃI�[�v��(fp) */
		fseek(fp, 0L, SEEK_END);			// �t�@�C���̍Ōォ��0�o�C�g�܂�Ō�ֈړ�
		fpoint = ftell(fp);					// �e�ʂ�ۑ�
		if( fpoint > 127L ){
			fseek(fp, -160L, SEEK_END);
			fgets(s,127,fp);				// 1�s(�ő�60�o�C�g�ɏ����}�[�W��)�̋�ǂ�
		}else{
			fseek(fp, 0L, SEEK_SET);		// �t�@�C���̐擪��
		}
		// �ŏI�s��ǂ�ŁA�������ݗ\��Ɠ������▢���̃f�[�^�����ɂ����return����
		fscanf(fp, "%d/%d/%d, %f, %f, %f\n", &yy,&mm,&dd,&value[0],&value[1],&value[2]);
		if(yy >= y && mm >= m && dd >= d ){
			fclose(fp);
			return(-1);
		}
		// �ŐV��1������(31��)�̃f�[�^��ǂݍ��ށB64�o�C�g�~31 = 1984�o�C�g
		if( ftell(fp) > 1984L ){
			fseek(fp, -1984L, SEEK_END);
			fgets(s,128,fp);				// 1�s�̋�ǂ�
		}else{
			fseek(fp, 0L, SEEK_SET);		// �t�@�C���̐擪��
		}
		// �������ݓ��Ɠ������́u�قȂ�����܂ށv�f�[�^��ǂ�
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
		if( (fp = fopen(file_name , "r")) ){		/* csv�f�[�^�����[�h�ŃI�[�v��(fp) */
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
			
			/*  �t�@�C�����̕ύX 1�f�[�^�`�� */
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
// data�t�H���_����csv�f�[�^������ �����s����
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
	
	float value[3][24];	// ���όv�Z�p�F�e���͒l
	float min,max;				// �ő�l
	float vmax, vmin;
	float vavr[3];
	int flag24=1;				// �f�[�^��24���ԕ������Ă��邩�ǂ���
	
	char name[17];					//  0123456
	char html_m[32];			// ���� 120923_ABC.csv.html

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
	
	/* html �o�� */
	if( (fp = fopen(csv , "r")) ){		/* csv�f�[�^�����[�h�ŃI�[�v��(fp) */
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
			if(!flag24){	// EOF�ƂȂ����� ������ł��ŏI�s�͓ǂ߂�͂��B
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
			fprintf(fp_html, "<center><h3>%d�N%d��%d��</h3></center><br><div align=right>[<a href=\"%s\">�O��</a>]�@�@[<a href=\"%s\">����</a>]�@�@[<a href=\"index.html\">�߂�</a>]<br></div><br>\r\n",y,m,d,html_b,html_m);
			fprintf(fp_html, "<h2>%s</h2>\r\n<center><table border=1>\r\n", name);
			
			for( row = 0 ; row < valn ; row++){
				fprintf(fp_html, "<tr><td colspan=24><b>(%d) ",row+1);
				switch( type[row] ){
					case LIGHT:		fprintf(fp_html, "�Ɠx[Lux]");	break;
					case TEMP:		fprintf(fp_html, "���x[��]");	break;
					case HUMIDITY:	fprintf(fp_html, "���x[��]");	break;
					case WATT:		fprintf(fp_html, "�d��[W]");	break;
					case BATT:		fprintf(fp_html, "�d��[V]");	break;
					case PRESS:		fprintf(fp_html, "�C��[hPa]");	break;
					case VALUE:		fprintf(fp_html, "�l");			break;
					case TIMES:		fprintf(fp_html, "��");		break;
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
			fprintf(fp_html, "[<a href=\"index.html\">�߂�</a>]�@�@[<a href=\"%s\">����</a>]�@�@[<a href=\"%s\">�O��</a>]<br>\r\n",html_m,html_b);
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
	char write[]    = "data/120923_0123456789ABCDF.csv";		// 31���� 32�o�C�g
	char html[]     = "htdocs/120923_0123456789ABCDF.csv.html";	// 38����+\0   39�o�C�g
	char html_b[]   = "120923_0123456789ABCDF.csv.html";	// ���̑O�̕� 31����   32�o�C�g
	char html_b2[]  = "120923_0123456789ABCDF.csv.html";	// ���ݕ�
	
	int y,m,d;		// ����̔N����
	unsigned long ymd;		// y * 12 * 31 + m * 31 + d;
	int yb,mb,db;	// �O��̔N����
	int yl,ml,dl;	// ���͂���� date_lim�̓W�J�l
	unsigned long ymd_l;		// y * 12 * 31 + m * 31 + d;
	int hh,hour;	// ����̎��A���̎�
	int hhb;		// �O��̎�
	int mm;
	int vals;		// 0�`3 ����̃f�[�^�����邩
	
	char s[128];	// ���������p
	float value[3][256];	// ���όv�Z�p�F�e���͒l
	float sum;				// ���όv�Z�p�F���v�l
	int div;				// ���όv�Z�p�F���Z�l
	int i,j;				// �ėp�ϐ�
	byte f;					// �ėp�t���O
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
			/* �f�[�^���� */
			fseek(fpr, 0L, SEEK_END);		// �t�@�C���̍Ōォ��0�o�C�g�܂�Ō�ֈړ�
			fpoint = ftell(fpr);			// ���̈ʒu���擾�B�܂�t�@�C���T�C�Y
			fpoint /= 2;					// �t�@�C���T�C�Y�̔�������
			if( strcmp(date_lim,"2000/01/01" ) ){	// 2000/01/01�ƈ�v���������i�ʏ퓮�쎞�j
				if( fpoint <= 172800L ) fseek(fpr, 0L, SEEK_SET);		// �t�@�C���̐擪�ɖ߂�
				else do{	// 172800�o�C�g�ȏ�̎� 60�f�[�^�~24���ԁ~60�o�C�g�~2 = 172800 (256�f�[�^�~60�~2=30720)
					fseek(fpr, -fpoint, SEEK_END);
					fgets(s,127,fpr);		// 1�s�̋�ǂ�
					fscanf(fpr,"%d/%d/%d,%d:%d,",&y,&m,&d,&hh,&mm);
					fgets(s,127,fpr);		// 1�s�̋�ǂ�
				//	printf("fp=%ld,y=%d,m=%d,d=%d\n",fpoint,y,m,d);	//�f�o�b�O�p
					ymd = (unsigned long)y * 12 * 31 + (unsigned long)m * 31 + (unsigned long)d;
					if( ymd >= ymd_l ){
						fseek(fpr, -fpoint, SEEK_CUR);
						fpoint=0;
					}else{
						fpoint /= 2;
					}
				}while( fpoint > 172800L );
			}else{
				fseek(fpr, 0L, SEEK_SET);		// �t�@�C���̐擪�ɖ߂�
				if( _delete_csv_daily_data(filename) == 0){
					printf("### delete file: data/%s\n",filename);
					fprintf(fpe,"delete file: data/%s\n",filename);
				}else{
					fprintf(fpe,"ERROR cannot delete file:data/%s\n",filename);
				}
			}
			/* �f�[�^�ǂݎ�菈�� */
			yb=0; mb=0; db=0; hhb=0; ymd=0UL;
			y=0;  m=0;  d= 0; hh=0;
			sprintf( html_b, "index.html");
			sprintf( html_b2, "index.html");
		//	printf("data read\n");	//�f�o�b�O�p
			do{	// �ǎ�P�t�@�C���̌J��Ԃ�
				/* �Ώۓ��̃f�[�^��T�� */
				if( db == 0 ){	// ���[�v�̏���
					while( ymd_l > ymd ){
						yb=y;	mb=m;	db=d;	hhb=hh;	//�ǂݎ��O�̃o�b�N�A�b�v
						if( fscanf(fpr,"%d/%d/%d,%d:%d,",&y,&m,&d,&hh,&mm) != EOF ){
						//	printf("ymd:%04d/%02d/%02d,%02d:%02d (%ld>%ld)\n", y, m, d, hh, mm, ymd_l, ymd);	// test	//�f�o�b�O�p
						//	fprintf(fpe,"ymd:%04d/%02d/%02d,%02d:%02d (%ld>%ld)\n", y, m, d, hh, mm, ymd_l, ymd);	// test	//�f�o�b�O�p
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
								�f�[�^�̖������Ԃ������܂����Ɛ�̓��̃f�[�^�������Ă��܂�
								6/22��4���`6/24��11���܂Ńf�[�^�����������ꍇ�F
								
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
						//	printf("done (ymd)\n");	//�f�o�b�O�p
							fprintf(fpe,"done (nothing to do)\n");
							printf("done (nothing to do)\n");
							fclose( fpr );
							fclose( fpe );
							return( 0 );
						}
					}
				}
				
				/* �������� �P����*/
				/* csv �o�� */
			//	printf("data write\n");	//�f�o�b�O�p
				sprintf(write, "data/%02d%02d%02d_%s", y-2000, m, d , filename );
				fprintf(fpe,"averaging  %s\n", write );
				if( (fpw = fopen(write , "w")) ){
					f = 1;	// �������̏������s���Ă��邱�Ƃ������t���O
					for( hour=0 ; hour < 24 ; hour++ ){	// ����������24���ԕ��̌J��Ԃ�
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
							/* �f�[�^�ǂݎ�� */
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
							
							/* ����̎����̓ǂݎ�� */
							yb=y;	mb=m;	db=d;	hhb=hh;	//�ǂݎ��O�̃o�b�N�A�b�v
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
								/* ���t����v���Ă��Ȃ��ꍇ��html�p�̓��t���X�V
									html_b		b2�̃o�b�N�A�b�v
									html_b2		���݂̃t�@�C����(�s��v�ɂȂ�O�̒l)�ɍX�V
								*/
								if( db != d && yb > 2000 ){
									sprintf( html_b, "%s" , html_b2 );
									sprintf( html_b2, "%02d%02d%02d_%s.html", yb-2000, mb, db , filename );
								}
								/* �ǎ撆�̓��t����v���Ă��Ȃ��ꍇ��hour���[�v�𔲂��� */
								if( yb != y || mb != m || db != d ){
									i = 256;		// i�̒l��vals��i+1���o�b�N�A�b�v����Ă���
									// hour = 24;	// �������̎������ς���Ă��܂��̂�hour�͕ύX�ł��Ȃ��B
									f = 0;			// hour���[�v�𔲂��邽�߂̃t���O����
									break;
								}
								/* �ǎ撆��hour���ƈ�v���Ă��Ȃ��ꍇ�ɂ������i���j���[�v�𔲂��� */
								if( hour != hh){
									i = 256;		// i�̒l��vals��i+1���o�b�N�A�b�v����Ă���
									break;
								}
							} else {
								/* �ǂݍ��݃t�@�C���̂d�n�e�ɂĕϊ��I������ */
							//	vals=i;
								/* CSV�f�[�^�o��(�Ō�̎�����) hour = 23 �̂Ƃ��A�������͒��f����Ă����� 23�ȉ������蓾�� */
								fprintf(fpw, "%04d/%02d/%02d,%02d:00", yb, mb, db, hour);
								for( j = 0 ; j < valn ; j++){
									fprintf(fpw, ", ");
									div = 0;
									sum = 0.;
									for( i = 0 ; i < vals ; i++ ){
										if( value[j][i] >= 0.0 ){
											if(	(j == 0 && val1 == TIMES) ||	// �f�[�^���u�񐔂̎��v
												(j == 1 && val2 == TIMES) ||
												(j == 2 && val3 == TIMES) ){
												sum += 1;
												div = 1;
											}else{							// �f�[�^���񐔈ȊO�̎�
												sum += value[j][i];
												div++;
											}
										}
									}
									// fprintf(fpe,"%s j=%d sum=%f div=%f\n", write, j ,sum, (float)div); //test
									if( div > 0 ) fprintf(fpw, "%f", ( sum / (float)div ) );
									else{
										fprintf(fpw, "-1.0" );		// div=0�̎��A�����Ȃ�̂�value���S��-1�̎�
										fprintf(fpe, "WARNING:vals=%d\n",vals);
									}
								}
								fprintf(fpw, "\n" );
								/* CSV�o�͂����܂� */
								
								fclose( fpw );
								fclose( fpr );
								/* html�o�� */
								sprintf( html, "htdocs/%02d%02d%02d_%s.html", yb-2000, mb, db , filename );
								fprintf(fpe,"html_out %s\n", html );
								
								/*		EOF�ŏI������ꍇ�́Ahtml_b2�̓��t���X�V����Ă��Ȃ��̂ŁA
										�O�X����html_b�ł͂Ȃ�html_b2�ɂȂ�B
										���̂��߁A csv2html�Łu�O���v��URL�w���html_b2���g���Ă���B */
								i = csv2html( write, html, valn , html_b2, type);
								if( i == -1 ) fprintf(fpe,"ERROR open for read :%s\n",write);
								if( i == -2 ) fprintf(fpe,"ERROR to write : %s\n", html );
								/* return ���� */
								fprintf(fpe,"done (EOF)\n");
								printf("done (EOF)\n");
								fclose( fpe );
								sprintf( html_b2, "%02d%02d%02d_%s.html", yb-2000, mb, db , filename );  // html_b2��CSV�f�[�^�o��(fpw)�̓��t��
								_html_jump( html_b2, filename);
								return( 0 );
							}
						}
						if( vals > 256) vals = 256;	// for(i)��256����Ă�ł��f�[�^���c���Ă��鎞�͎̂Ă�
						
						/* CSV�f�[�^�o��(�����P��) */
						fprintf(fpw, "%04d/%02d/%02d,%02d:00", yb, mb, db, hour);
						for( j = 0 ; j < valn ; j++){
							fprintf(fpw, ", ");
							div = 0;
							sum = 0.;
							for( i = 0 ; i < vals ; i++ ){
								if( value[j][i] >= 0.0 ){
									if(	(j == 0 && val1 == TIMES) ||	// �f�[�^���u�񐔂̎��v
										(j == 1 && val2 == TIMES) ||
										(j == 2 && val3 == TIMES) ){
										sum += 1;
										div = 1;
									}else{							// �f�[�^���񐔈ȊO�̎�
										sum += value[j][i];
										div++;
									}
								}
							}
							// fprintf(fpe,"%s j=%d sum=%f div=%f\n", write, j ,sum, (float)div); //test
							if( div > 0 ) fprintf(fpw, "%f", ( sum / (float)div ) );
							else{
								fprintf(fpw, "-1.0" );		// div=0�̎��A�����Ȃ�̂�value���S��-1�̎�
								fprintf(fpe, "WARNING:vals=%d\n",vals);
							}
						}
						fprintf(fpw, "\n" );
						/* �o�b�t�@256�𒴂����ꍇ�̓ǂݎ̂ď��� */
						if( vals == 256 ){
							fprintf(fpe, "WARNING:vals=%d\n",vals);
							do{
								fscanf(fpr,"%[^\r\n]\n", s);
								yb=y;	mb=m;	db=d;	hhb=hh;	//�ǂݎ��O�̃o�b�N�A�b�v
								if( fscanf(fpr,"%d/%d/%d,%d:%d,",&y,&m,&d,&hh,&mm) != EOF ){
									if( yb != y || mb != m || db != d ) hhb = -1;
								}else hhb = -1;
							}while( hh == hhb);		// �������Ԃ̂��͓̂ǂݎ̂Ă�
						}
						/* ���t���ς���Ă�������break���� */
						if( f == 0 ) break;
					}
					fclose( fpw );
					fprintf(fpe,"exported   %s\n", write );
				}else{
					fprintf(fpe,"ERROR open for write :%s\n",write);
					break;
				}
				// fprintf(fpe, "WARNING:unwanted data existed befor EOF\n");
				/* html�o�� �B
				���t�̕ς��ڂ�html�o�͂��镔���B
				���X�̃o�b�`�����ł́A���̕ς��ڂ̌�Ƀf�[�^�������̂Œʂ�Ȃ��B
				����̃o�b�`�����ł́A�S���ϊ��Ȃ̂ŁA�����ς�邽�тɒʂ� */
				sprintf( html, "htdocs/%02d%02d%02d_%s.html", yb-2000, mb, db , filename );
				fprintf(fpe,"html_out %s\n", html );
				
				/*		23���܂ł����Ȃ������ꍇ�́A�H�H�H
				*/
				i=csv2html( write, html, valn , html_b, type);
				if( i == -1 ) fprintf(fpe,"ERROR open for read :%s\n",write);
				if( i == -2 ) fprintf(fpe,"ERROR to write : %s\n", html );
		//	}while( feof(fpr) != EOF );
			}while( feof(fpr) == 0 );
			fclose( fpr );
			sprintf( html_b2, "%02d%02d%02d_%s.html", yb-2000, mb, db , filename );  // html_b2��CSV�f�[�^�o��(fpw)�̓��t��
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
