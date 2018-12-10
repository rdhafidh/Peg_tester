#include "xlswriter.h"

XlsWriter::XlsWriter()
{
    
}

bool XlsWriter::writeReport(const QString &filename , std::deque<std::pair<int, double> > d, const std::pair<QString,QString> & compiler, const QString &satuan)
{
    QTXLSX_USE_NAMESPACE
            
            std::vector<std::pair<int, double> > mdata;
            std::move(d.begin()
                      ,d.end(),
                      std::back_inserter(mdata));
    
     Document doc(filename);
     Format fmtjudul,fmtangka,fmtsubthreadclang,
            fmtsubnonthreadclang,fmtsubthreadmingw,fmtsubnonthreadmingw;
    fmtjudul.setBorderStyle( Format::BorderMedium);
    fmtjudul.setHorizontalAlignment( Format::AlignHCenter);
    fmtjudul.setVerticalAlignment( Format::AlignVCenter);
    fmtangka.setBorderStyle( Format::BorderMedium);
    fmtsubthreadclang.setBorderStyle( Format::BorderMedium);
    fmtsubnonthreadclang.setBorderStyle( Format::BorderMedium);
    fmtsubthreadmingw.setBorderStyle( Format::BorderMedium);
    fmtsubnonthreadmingw.setBorderStyle( Format::BorderMedium);
    
    
    if (satuan==QString("nano")){
        doc.write("B3","Perhitungan nano second");            
    }
    if (satuan==QString("mili")){
        doc.write("B3","Perhitungan mili second");            
    }
    if (satuan==QString("detik")){
        doc.write("B3","Perhitungan perdetik");            
    }
    
    doc.mergeCells("B3:F3",fmtjudul);
    
    doc.write("B4","No");
    doc.mergeCells("B4:B6",fmtjudul);
    
    doc.write("C4","Benchmark Test PEG Parsing");
    doc.mergeCells("C4:F4",fmtjudul);
    
	if (compiler.first.contains("VS2015",Qt::CaseInsensitive)){
    doc.write("C5","VS2015");
	}
	if (compiler.first.contains("mingw",Qt::CaseInsensitive)){
	  doc.write("C5","mingw Posix GCC 5.1");
	}
	if (compiler.first.contains("clang",Qt::CaseInsensitive)){
	  doc.write("C5","clang 3.6");
	}	
	if (compiler.first.contains("gcc",Qt::CaseInsensitive)){
	  doc.write("C5","g++ 4.9");
	}
	
    doc.mergeCells("C5:D5",fmtjudul);
    
	if (compiler.second.contains("mingw",Qt::CaseInsensitive)){
    doc.write("E5","MinGW Posix GCC 5.1");
	}
	if (compiler.second.contains("clang",Qt::CaseInsensitive)){
	  doc.write("E5","clang 3.6");
	}
	if (compiler.second.contains("VS2015",Qt::CaseInsensitive)){
	  doc.write("E5","VS2015");
	}
	if (compiler.second.contains("gcc",Qt::CaseInsensitive)){
	  doc.write("E5","g++ 4.9 "); 
	}
    doc.mergeCells("E5:F5",fmtjudul);
    
    doc.write("C6","Multithread",fmtsubthreadclang);
    doc.write("D6","Non-Multithread",fmtsubnonthreadclang);
    
    doc.write("E6","Multithread",fmtsubthreadmingw);
    doc.write("F6","Non-Multithread",fmtsubnonthreadmingw);
    
    //write no
    
    QStringList mnum,m_vs,n_vs,m_mw,n_mw; 
    QVector<int> mbase,steppoint;
    mbase.resize(mdata.size());
    steppoint.resize(4+1); //4 step
    genNumber(mbase.begin(),mbase.end(),7,1);//starting from B7
    
    //qDebug()<<"mbase:"<<mbase;
    for (int in=0;in<mbase.size()/4;in++){
        mnum<< QString("B%1").arg(mbase.at(in));
    }
    for(int in1=0;in1<mbase.size();in1++){
        m_vs<< QString("C%1").arg(mbase.at(in1));
    }
    for(int in2=0;in2<mbase.size();in2++){
       n_vs<< QString("D%1").arg(mbase.at(in2));
    }
    for(int in3=0;in3<mbase.size();in3++){
       m_mw<< QString("E%1").arg(mbase.at(in3));
    }
    for(int in4=0;in4<mbase.size();in4++){
       n_mw<< QString("F%1").arg(mbase.at(in4));
    }
          
    std::vector<std::pair<int, double> > n_mvs,n_nvs,n_mmw,n_nmw;
    n_mvs.resize(mdata.size()/4);
    n_nvs.resize(mdata.size()/4);
    n_mmw.resize(mdata.size()/4);
    n_nmw.resize(mdata.size()/4);
    genNumber(steppoint.begin(),steppoint.end(),0,(int)mdata.size()/4);    
    std::copy_n(mdata.begin(),steppoint.at(1),std:: inserter(n_mvs,n_mvs.begin()));
    std::copy_n(mdata.begin()+steppoint.at(1),steppoint.at(2),std:: inserter(n_nvs,n_nvs.begin()));
    std::copy_n(mdata.begin()+steppoint.at(2),steppoint.at(3),std:: inserter(n_mmw,n_mmw.begin()));
    std::copy_n(mdata.begin()+steppoint.at(3),steppoint.at(4),std:: inserter(n_nmw,n_nmw.begin()));
    mdata.clear();
    
    //qDebug()<<"mnum[mbase]: "<<mnum;
     QLocale loc(QLocale::Indonesian);
    for (int mnums=0;mnums<mnum.size();mnums++){
        doc.write(mnum.at(mnums).toUtf8().data(),mnums+1,fmtangka);
          
         doc.write(m_vs.at(mnums).toUtf8().data(),loc.toString(n_mvs.at(mnums).second).toUtf8().data(),fmtangka);
          doc.write(n_vs.at(mnums).toUtf8().data(),loc.toString(n_nvs.at(mnums).second).toUtf8().data() ,fmtangka);
           doc.write(m_mw.at(mnums).toUtf8().data(),loc.toString(n_mmw.at(mnums).second).toUtf8().data(),fmtangka);
            doc.write(n_mw.at(mnums).toUtf8().data(),loc.toString(n_nmw.at(mnums).second).toUtf8().data() ,fmtangka);
    }
   
    //write rata2
    std::pair<int,float> rata_n_mvs=std::accumulate(n_mvs.begin(),n_mvs.end(),std::make_pair(0,0.000),[]( std::pair<int,double> lhs, std::pair<int,double> rhs)  
    {
        return std::make_pair(0,lhs.second+rhs.second);
    });
      
    std::pair<int,float>  rata_n_nvs=std::accumulate(n_nvs.begin(),n_nvs.begin()+mnum.size(),std::make_pair(0,0.000),[]( std::pair<int,double> lhs, std::pair<int,double> rhs)  
    {
        return std::make_pair(0,lhs.second+rhs.second);
    });
    std::pair<int,float>  rata_n_mmw=std::accumulate(n_mmw.begin(),n_mmw.begin()+mnum.size(),std::make_pair(0,0.000),[]( std::pair<int,double> lhs, std::pair<int,double> rhs)  
    {
        return std::make_pair(0,lhs.second+rhs.second);
    });
    std::pair<int,float> rata_n_nmw=std::accumulate(n_nmw.begin(),n_nmw.begin()+mnum.size(),std::make_pair(0,0.000),[]( std::pair<int,double> lhs, std::pair<int,double> rhs)  
    {
        return std::make_pair(0,lhs.second+rhs.second);
    });
    doc.write(QString("B%1").arg(mbase.size()/4+7),"Rata^",fmtangka);
    
    doc.write(QString("C%1").arg(mbase.size()/4+7),loc.toString(rata_n_mvs.second/mnum.size()).toStdString().c_str(),fmtangka);
    doc.write(QString("D%1").arg(mbase.size()/4+7),loc.toString(rata_n_nvs.second/mnum.size()).toStdString().c_str(),fmtangka);
    doc.write(QString("E%1").arg(mbase.size()/4+7),loc.toString(rata_n_mmw.second/mnum.size()).toStdString().c_str(),fmtangka);
    doc.write(QString("F%1").arg(mbase.size()/4+7),loc.toString(rata_n_nmw.second/mnum.size()).toStdString().c_str(),fmtangka);
    
    
    return doc.save();
}

bool XlsWriter::writeReportSingleBenchmark(const QString &filename, std::deque<std::pair<int, double> > d,const  QString & compiler,const QString & satuan)
{
    //workaraound single compiler test
    QTXLSX_USE_NAMESPACE
            
            std::deque<  std::pair<int,double> > mdata;
            std::move(d.begin()
                      ,d.end(),
                      std::back_inserter(mdata));
    
     Document doc(filename);
     Format fmtjudul,fmtangka,fmtsubthreadclang,
            fmtsubnonthreadclang;
    fmtjudul.setBorderStyle( Format::BorderMedium);
    fmtangka.setBorderStyle( Format::BorderMedium);
    fmtsubthreadclang.setBorderStyle( Format::BorderMedium);
    fmtsubnonthreadclang.setBorderStyle( Format::BorderMedium);
    fmtjudul.setHorizontalAlignment( Format::AlignHCenter);
    fmtjudul.setVerticalAlignment( Format::AlignVCenter);
    
    if (satuan==QString("nano")){
        doc.write("B3","Perhitungan nano second");            
    }
    if (satuan==QString("mili")){
        doc.write("B3","Perhitungan mili second");            
    }
    if (satuan==QString("detik")){
        doc.write("B3","Perhitungan perdetik");            
    }
    doc.mergeCells("B3:D3",fmtjudul);
    
    doc.write("B4","No");
    doc.mergeCells("B4:B6",fmtjudul);
    
    doc.write("C4","Benchmark Test PEG Parsing");
    doc.mergeCells("C4:D4",fmtjudul);
    
	if (compiler.contains("clang",Qt::CaseInsensitive)){
	  doc.write("C5","clang 3.6");
	} 
	if (compiler.contains("VS2015",Qt::CaseInsensitive)){
	  doc.write("C5","VS2015");
	}
	if (compiler.contains("mingw",Qt::CaseInsensitive)){
	  doc.write("C5","MinGW Posix 5.1");
	}
	if (compiler.contains("gcc",Qt::CaseInsensitive)){
	  doc.write("C5","g++ 4.9");
	}
    doc.mergeCells("C5:D5",fmtjudul);
     
    doc.write("C6","Multithread",fmtsubthreadclang);
    doc.write("D6","Non-Multithread",fmtsubnonthreadclang);
    
    //write no
    
    QStringList mnum,m_vs,n_vs; 
    QVector<int> mbase,steppoint;
    mbase.resize(mdata.size());
    steppoint.resize(2+1); //2 step
    genNumber(mbase.begin(),mbase.end(),7,1);//starting from B7
    
    //qDebug()<<"mbase:"<<mbase;
    for (int in=0;in<mbase.size()/2;in++){
        mnum<< QString("B%1").arg(mbase.at(in));
    }
    for(int in1=0;in1<mbase.size();in1++){
        m_vs<< QString("C%1").arg(mbase.at(in1));
    }
    for(int in2=0;in2<mbase.size();in2++){
       n_vs<< QString("D%1").arg(mbase.at(in2));
    }
          
    std::deque< std::pair<int, double> > n_mvs,n_nvs;
    n_mvs.resize(mdata.size()/2);
    n_nvs.resize(mdata.size()/2);
    
    genNumber(steppoint.begin(),steppoint.end(),0,(int)mdata.size()/2);     
      
    std::copy_n(mdata.begin(),steppoint.at(1),std:: inserter(n_mvs,n_mvs.begin()));
    std::copy(mdata.begin()+steppoint.at(1), mdata.end(),
                 std::inserter(n_nvs,n_nvs.begin()));
     
     QLocale loc(QLocale::Indonesian);
     mdata.clear();
    for (int mnums=0;mnums<mnum.size();mnums++){
        doc.write(mnum.at(mnums).toUtf8().data(),mnums+1,fmtangka);
       
         doc.write(m_vs.at(mnums).toUtf8().data(),loc.toString(n_mvs.at(mnums).second).toUtf8().data(),fmtangka);
         
         doc.write(n_vs.at(mnums).toUtf8().data(),loc.toString(n_nvs.at(mnums).second).toUtf8().data() ,fmtangka); 
    }
	
    std::pair<int,float> rata_n_mvs=std::accumulate(n_mvs.begin(),n_mvs.begin()+mnum.size(),std::make_pair(0,0.000),[]( std::pair<int,double> lhs, std::pair<int,double> rhs)  
    {
        return std::make_pair(0,lhs.second+rhs.second);
    });
      
    std::pair<int,float>  rata_n_nvs=std::accumulate(n_nvs.begin(),n_nvs.begin()+mnum.size(),std::make_pair(0,0.000),[]( std::pair<int,double> lhs, std::pair<int,double> rhs)  
    {
        return std::make_pair(0,lhs.second+rhs.second);
    });
    
    doc.write(QString("B%1").arg(mbase.size()/2+7),"Rata^",fmtangka);
    
    doc.write(QString("C%1").arg(mbase.size()/2+7),loc.toString(rata_n_mvs.second/mnum.size()).toStdString().c_str(),fmtangka);
    doc.write(QString("D%1").arg(mbase.size()/2+7),loc.toString(rata_n_nvs.second/mnum.size()).toStdString().c_str(),fmtangka);
    
    return doc.save();
}
