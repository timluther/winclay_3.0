//TWINf basic widgets code

#define CMDperlinchanged 900
#define CMDredrawperlin 901

class imageview:public tview
{
public:
	tsurface *img;
	imageview(INT32 x,INT32 y,tsurface *img_):tview(x,y,x+img_->size.x-1,y+img_->size.y-1)
	{
		growmode=TGdragonx1|TGdragony1;
		img=img_;
	}
	virtual void draw()
	{
		putbitmap(a.x,a.y,img,0);
	}
	
};

class perlinview:public tgroup
{
    public:
    tcolour palette[256];
    
    numbox *freqx,*freqy,*persistance,*xoff,*yoff;
    UINT32 drawmode;    
    imageview *image;
    flt *heightfield;
    perlinview(INT32 x,INT32 y);
    virtual void draw();
    virtual void handleevent(tevent *event);
    void updatepattern();
};

flt tperlinnoise_2d(flt x, flt y,flt xfreq,flt yfreq,flt p);



